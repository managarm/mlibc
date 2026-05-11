#include <assert.h>
#include <shadow.h>
#include <stddef.h>
#include <string.h>

int main(void) {
	struct spwd *sp;

	const char *valid = "user1:$6$salt$hash:18500:0:99999:7:30:19000:123";
	sp = sgetspent(valid);
	assert(sp != NULL);
	assert(strcmp(sp->sp_namp, "user1") == 0);
	assert(strcmp(sp->sp_pwdp, "$6$salt$hash") == 0);
	assert(sp->sp_lstchg == 18500);
	assert(sp->sp_min == 0);
	assert(sp->sp_max == 99999);
	assert(sp->sp_warn == 7);
	assert(sp->sp_inact == 30);
	assert(sp->sp_expire == 19000);
	assert(sp->sp_flag == 123);

	const char *empty_fields = "managarm:WJg36EZUlJ96k:19971:0:99999:7:::";
	sp = sgetspent(empty_fields);
	assert(sp != NULL);
	assert(strcmp(sp->sp_namp, "managarm") == 0);
	assert(strcmp(sp->sp_pwdp, "WJg36EZUlJ96k") == 0);
	assert(sp->sp_lstchg == 19971);
	assert(sp->sp_min == 0);
	assert(sp->sp_max == 99999);
	assert(sp->sp_warn == 7);
	assert(sp->sp_inact == -1);
	assert(sp->sp_expire == -1);
	assert(sp->sp_flag == -1UL);

	const char *invalid_few_fields = "user3:*:18500";
	sp = sgetspent(invalid_few_fields);
	assert(sp == NULL);

	const char *invalid_no_colons = "just_a_string";
	sp = sgetspent(invalid_no_colons);
	assert(sp == NULL);

	const char *empty_str = "";
	sp = sgetspent(empty_str);
	assert(sp == NULL);

	return 0;
}
