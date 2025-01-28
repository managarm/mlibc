/*
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD
 *
 * code taken from OPNSense, with modifications
 *
 * Copyright (c) 2002 Tim J. Robbins.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <wordexp.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <stdint.h>

#define SHELL_PATH "/bin/sh"
#define SHELL_NAME "sh"

static size_t we_read_fully(int fd, char *buffer, size_t len) {
	size_t done = 0;

	do {
		ssize_t nread = read(fd, buffer + done, len - done);
		if(nread == -1 && errno == EINTR)
			continue;
		if(nread <= 0)
			break;
		done += nread;
	} while (done != len);

	return done;
}

static int we_askshell(const char *words, wordexp_t *we, int flags) {
	int pdes[2]; /* pipe to child */
	char bbuf[9]; /* buffer for byte count */
	char wbuf[9]; /* buffer for word count */
	size_t nwords = 0; /* number of words from child */
	size_t nbytes = 0; /* number of bytes from child */
	size_t sofs = 0; /* offset into we->we_strings */
	size_t vofs = 0; /* offset into we->we_wordv */
	pid_t pid; /* PID of child */
	pid_t wpid; /* waitpid return value */
	int status; /* child exit status */
	int error; /* our return value */
	int serrno; /* errno to return */
	char *np, *p; /* handy pointers */
	char *nstrings; /* temporary for realloc() */
	char **new_wordv; /* temporary for realloc() */
	sigset_t newsigblock;
	sigset_t oldsigblock;
	const char *ifs = getenv("IFS");

	serrno = errno;

	if(pipe2(pdes, O_CLOEXEC) < 0)
		return WRDE_NOSPACE;

	(void)sigemptyset(&newsigblock);
	(void)sigaddset(&newsigblock, SIGCHLD);
	(void)sigprocmask(SIG_BLOCK, &newsigblock, &oldsigblock);

	if((pid = fork()) < 0) {
		serrno = errno;
		close(pdes[0]);
		close(pdes[1]);
		(void)sigprocmask(SIG_SETMASK, &oldsigblock, NULL);
		errno = serrno;
		return WRDE_NOSPACE;
	} else if(pid == 0) {
		/*
		 * We are the child; make /bin/sh expand `words'.
		 */
		(void)sigprocmask(SIG_SETMASK, &oldsigblock, NULL);
		if((pdes[1] != STDOUT_FILENO ? dup2(pdes[1], STDOUT_FILENO) : fcntl(pdes[1], F_SETFD, 0)) < 0)
			_exit(1);

		execl(SHELL_PATH, SHELL_NAME, flags & WRDE_UNDEF ? "-u" : "+u",
			"-c", "IFS=$1;eval \"$2\";eval \"set -- $3\";IFS=;a=\"$*\";"
			"printf '%08x' \"$#\" \"${#a}\";printf '%s\\0' \"$@\"", "",
			ifs != NULL ? ifs : " \t\n",
			flags & WRDE_SHOWERR ? "" : "exec 2>/dev/null",
			words,
			(char *)NULL);
		_exit(1);
	}

	/*
	 * We are the parent; read the output of the shell wordexp function,
	 * which is a 32-bit hexadecimal word count, a 32-bit hexadecimal
	 * byte count (not including terminating null bytes), followed by
	 * the expanded words separated by nulls.
	 */
	close(pdes[1]);
	if(we_read_fully(pdes[0], wbuf, 8) != 8 || we_read_fully(pdes[0], bbuf, 8) != 8) {
		error = flags & WRDE_UNDEF ? WRDE_BADVAL : WRDE_SYNTAX;
		serrno = errno;
		goto cleanup;
	}
	wbuf[8] = bbuf[8] = '\0';
	nwords = strtol(wbuf, NULL, 16);
	nbytes = strtol(bbuf, NULL, 16) + nwords;

	/*
	 * Allocate or reallocate (when flags & WRDE_APPEND) the word vector
	 * and string storage buffers for the expanded words we're about to
	 * read from the child.
	 */
	sofs = we->we_nbytes;
	vofs = we->we_wordc;
	if((flags & (WRDE_DOOFFS|WRDE_APPEND)) == (WRDE_DOOFFS | WRDE_APPEND))
		vofs += we->we_offs;
	we->we_wordc += nwords;
	we->we_nbytes += nbytes;

	if((new_wordv = (char **) realloc(we->we_wordv, (we->we_wordc + 1 + (flags & WRDE_DOOFFS ?  we->we_offs : 0)) * sizeof(char *))) == NULL) {
		error = WRDE_NOSPACE;
		goto cleanup;
	}

	we->we_wordv = new_wordv;

	if((nstrings = (char *) realloc(we->we_strings, we->we_nbytes)) == NULL) {
		error = WRDE_NOSPACE;
		goto cleanup;
	}

	for(size_t i = 0; i < vofs; i++) {
		if(we->we_wordv[i] != NULL)
			we->we_wordv[i] += nstrings - we->we_strings;
	}
	we->we_strings = nstrings;

	if(we_read_fully(pdes[0], we->we_strings + sofs, nbytes) != nbytes) {
		error = flags & WRDE_UNDEF ? WRDE_BADVAL : WRDE_SYNTAX;
		serrno = errno;
		goto cleanup;
	}

	error = 0;
cleanup:
	close(pdes[0]);

	do {
		wpid = waitpid(pid, &status, 0);
	} while(wpid < 0 && errno == EINTR);

	(void)sigprocmask(SIG_SETMASK, &oldsigblock, NULL);

	if(error != 0) {
		errno = serrno;
		return error;
	}

	if(wpid < 0 || !WIFEXITED(status) || WEXITSTATUS(status) != 0)
		return flags & WRDE_UNDEF ? WRDE_BADVAL : WRDE_SYNTAX;

	/*
	 * Break the null-terminated expanded word strings out into
	 * the vector.
	 */
	if(vofs == 0 && flags & WRDE_DOOFFS) {
		while (vofs < we->we_offs)
			we->we_wordv[vofs++] = NULL;
	}

	p = we->we_strings + sofs;
	while (nwords-- != 0) {
		we->we_wordv[vofs++] = p;
		if((np = (char *) memchr(p, '\0', nbytes)) == NULL)
			return WRDE_NOSPACE;

		nbytes -= np - p + 1;
		p = np + 1;
	}

	we->we_wordv[vofs] = NULL;
	return 0;
}

/*
 * we_check --
 *	Check that the string contains none of the following unquoted
 *	special characters: <newline> |&;<>(){}
 *	or command substitutions when WRDE_NOCMD is set in flags.
 */
static int we_check(const char *words, int flags)
{
	char c;
	int dquote, level, quote, squote;

	quote = squote = dquote = 0;
	while ((c = *words++) != '\0') {
		switch (c) {
			case '\\': {
				if(squote == 0)
					quote ^= 1;
				continue;
			}
			case '\'': {
				if(quote + dquote == 0)
					squote ^= 1;
				break;
			}
			case '"': {
				if(quote + squote == 0)
					dquote ^= 1;
				break;
			}
			case '`': {
				if(quote + squote == 0 && flags & WRDE_NOCMD)
					return WRDE_CMDSUB;
				while ((c = *words++) != '\0' && c != '`')
					if(c == '\\' && (c = *words++) == '\0')
						break;
				if(c == '\0')
					return WRDE_SYNTAX;
				break;
			}
			case '|':
			case '&':
			case ';':
			case '<':
			case '>':
			case '{':
			case '}':
			case '(':
			case ')':
			case '\n': {
				if(quote + squote + dquote == 0)
					return WRDE_BADCHAR;
				break;
			}
			case '$': {
				if((c = *words++) == '\0')
					break;
				else if(quote + squote == 0 && c == '(') {
					if(flags & WRDE_NOCMD && *words != '(')
						return WRDE_CMDSUB;
					level = 1;
					while ((c = *words++) != '\0') {
						if(c == '\\') {
							if((c = *words++) == '\0')
								break;
						} else if(c == '(')
							level++;
						else if(c == ')' && --level == 0)
							break;
					}
					if(c == '\0' || level != 0)
						return WRDE_SYNTAX;
				} else if(quote + squote == 0 && c == '{') {
					level = 1;
					while ((c = *words++) != '\0') {
						if(c == '\\') {
							if((c = *words++) == '\0')
								break;
						} else if(c == '{')
							level++;
						else if(c == '}' && --level == 0)
							break;
					}
					if(c == '\0' || level != 0)
						return WRDE_SYNTAX;
				} else
					--words;
				break;
			}
			default: {
				break;
			}
		}
		quote = 0;
	}

	if(quote + squote + dquote != 0)
		return WRDE_SYNTAX;

	return 0;
}

int wordexp(const char * __restrict words, wordexp_t * __restrict we, int flags) {
	int error;

	if(flags & WRDE_REUSE)
		wordfree(we);

	if((flags & WRDE_APPEND) == 0) {
		we->we_wordc = 0;
		we->we_wordv = NULL;
		we->we_strings = NULL;
		we->we_nbytes = 0;
	}

	if((error = we_check(words, flags)) != 0) {
		wordfree(we);
		return error;
	}

	if((error = we_askshell(words, we, flags)) != 0) {
		wordfree(we);
		return error;
	}

	return 0;
}

void wordfree(wordexp_t *we) {
	if (we == NULL)
		return;
	free(we->we_wordv);
	free(we->we_strings);
	we->we_wordv = NULL;
	we->we_strings = NULL;
	we->we_nbytes = 0;
	we->we_wordc = 0;
}
