use bindgen::callbacks::{AttributeInfo, ParseCallbacks, TypeKind};
use clap::Parser;
use quote::quote;
use saphyr::{Yaml, LoadableYamlNode};
use std::collections::{HashMap, HashSet};
use std::fs;
use std::io::Write;
use std::path::{Path, PathBuf};
use std::process::{Command, Stdio};
use syn::visit::{self, Visit};
use syn::{parse_file, Item};

#[derive(Parser, Debug)]
#[command(author, version, about = "Auto-generate libc target bindings")]
struct Args {
	/// Path to the new OS's mod.rs file (e.g. "src/unix/linux_like/myos/mod.rs")
	#[arg(short = 'm', long)]
	mod_path: PathBuf,

	/// Path to the directory where the target headers are installed
	#[arg(short = 'i', long)]
	headers_dir: PathBuf,

	/// Path to YAML configuration file containing whitelists
	#[arg(short = 'c', long)]
	config: PathBuf,

	/// Path to a file containing code to prepend to the generated mod.rs
	#[arg(short = 'p', long)]
	prepend_file: PathBuf,

	/// Target triple to pass to clang/bindgen
	#[arg(short = 't', long)]
	target: String,

	/// Additional include directories for clang/bindgen
	#[arg(short = 'I', long = "include-dir")]
	include_dirs: Vec<PathBuf>,
}

fn map_mlibc_type(name: &str) -> Option<&'static str> {
	match name {
		"__mlibc_int64" | "__s64" => Some("i64"),
		"__mlibc_uint64" | "__u64" => Some("u64"),
		"__mlibc_int32" | "__s32" => Some("i32"),
		"__mlibc_uint32" | "__u32" => Some("u32"),
		"__mlibc_int16" | "__s16" => Some("i16"),
		"__mlibc_uint16" | "__u16" => Some("u16"),
		"__mlibc_int8" | "__s8" => Some("i8"),
		"__mlibc_uint8" | "__u8" => Some("u8"),
		"__mlibc_intptr" => Some("isize"),
		"__mlibc_uintptr" => Some("usize"),
		"__mlibc_size" => Some("usize"),
		"__mlibc_ssize" => Some("isize"),
		"__mlibc_ptrdiff" => Some("isize"),
		_ => None,
	}
}

#[derive(Debug, Default)]
struct WhitelistConfig {
	functions: HashSet<String>,
	structs: HashSet<String>,
	unions: HashSet<String>,
	constants: HashSet<String>,
	types: HashSet<String>,
	statics: HashSet<String>,
	opaque_structs: HashSet<String>,
	const_types: HashMap<String, String>,
	architectures: Vec<String>,
}

struct Whitelist {
	config: WhitelistConfig,
	used_functions: std::cell::RefCell<HashSet<String>>,
	used_structs: std::cell::RefCell<HashSet<String>>,
	used_unions: std::cell::RefCell<HashSet<String>>,
	used_constants: std::cell::RefCell<HashSet<String>>,
	used_types: std::cell::RefCell<HashSet<String>>,
	used_statics: std::cell::RefCell<HashSet<String>>,
	used_opaque_structs: std::cell::RefCell<HashSet<String>>,
}

impl Whitelist {
	fn load(path: &Path) -> Result<Self, Box<dyn std::error::Error>> {
		let content = fs::read_to_string(path)?;
		let docs = Yaml::load_from_str(&content)?;
		let doc = docs.get(0).ok_or("Empty YAML document")?;

		let mut functions = HashSet::new();
		let mut structs = HashSet::new();
		let mut unions = HashSet::new();
		let mut constants = HashSet::new();
		let mut types = HashSet::new();
		let mut statics = HashSet::new();
		let mut opaque_structs = HashSet::new();
		let mut const_types = HashMap::new();
		let mut architectures = Vec::new();

		if let Some(hash) = doc.as_mapping() {
			for (k, v) in hash {
				let key_str = k.as_str().ok_or_else(|| format!("Config key '{:?}' is not a string", k))?;
				match key_str {
					"functions" => {
						if let Some(arr) = v.as_vec() {
							for item in arr {
								if let Some(s) = item.as_str() {
									functions.insert(s.to_string());
								}
							}
						}
					}
					"structs" => {
						if let Some(arr) = v.as_vec() {
							for item in arr {
								if let Some(s) = item.as_str() {
									structs.insert(s.to_string());
								}
							}
						}
					}
					"unions" => {
						if let Some(arr) = v.as_vec() {
							for item in arr {
								if let Some(s) = item.as_str() {
									unions.insert(s.to_string());
								}
							}
						}
					}
					"constants" => {
						if let Some(arr) = v.as_vec() {
							for item in arr {
								if let Some(s) = item.as_str() {
									constants.insert(s.to_string());
								}
							}
						}
					}
					"types" => {
						if let Some(arr) = v.as_vec() {
							for item in arr {
								if let Some(s) = item.as_str() {
									types.insert(s.to_string());
								}
							}
						}
					}
					"statics" => {
						if let Some(arr) = v.as_vec() {
							for item in arr {
								if let Some(s) = item.as_str() {
									statics.insert(s.to_string());
								}
							}
						}
					}
					"opaque_structs" => {
						if let Some(arr) = v.as_vec() {
							for item in arr {
								if let Some(s) = item.as_str() {
									opaque_structs.insert(s.to_string());
								}
							}
						}
					}
					"architectures" => {
						if let Some(arr) = v.as_vec() {
							for item in arr {
								if let Some(s) = item.as_str() {
									architectures.push(s.to_string());
								}
							}
						}
					}
					"const_types" => {
						if let Some(ct_hash) = v.as_mapping() {
							for (ct_k, ct_v) in ct_hash {
								if let (Some(ct_k_str), Some(ct_v_str)) = (ct_k.as_str(), ct_v.as_str()) {
									const_types.insert(ct_k_str.to_string(), ct_v_str.to_string());
								}
							}
						}
					}
					_ => {
						return Err(format!("Unknown config key '{}' in config file", key_str).into());
					}
				}
			}
		}

		let config = WhitelistConfig {
			functions,
			structs,
			unions,
			constants,
			types,
			statics,
			opaque_structs,
			const_types,
			architectures,
		};

		Ok(Self {
			config,
			used_functions: std::cell::RefCell::new(HashSet::new()),
			used_structs: std::cell::RefCell::new(HashSet::new()),
			used_unions: std::cell::RefCell::new(HashSet::new()),
			used_constants: std::cell::RefCell::new(HashSet::new()),
			used_types: std::cell::RefCell::new(HashSet::new()),
			used_statics: std::cell::RefCell::new(HashSet::new()),
			used_opaque_structs: std::cell::RefCell::new(HashSet::new()),
		})
	}


	fn is_requested(&self, name: &str) -> bool {
		self.config.structs.contains(name)
			|| self.config.opaque_structs.contains(name)
			|| self.config.unions.contains(name)
			|| self.config.types.contains(name)
	}

	fn should_keep_struct(&self, name: &str) -> bool {
		if self.is_requested(name) {
			if self.config.structs.contains(name) {
				self.used_structs.borrow_mut().insert(name.to_string());
			}
			if self.config.opaque_structs.contains(name) {
				self.used_opaque_structs.borrow_mut().insert(name.to_string());
			}
			if self.config.types.contains(name) {
				self.used_types.borrow_mut().insert(name.to_string());
			}
			true
		} else {
			false
		}
	}

	fn should_keep_union(&self, name: &str) -> bool {
		if self.is_requested(name) {
			if self.config.unions.contains(name) {
				self.used_unions.borrow_mut().insert(name.to_string());
			}
			if self.config.types.contains(name) {
				self.used_types.borrow_mut().insert(name.to_string());
			}
			true
		} else {
			false
		}
	}

	fn should_keep_type(&self, name: &str) -> bool {
		if map_mlibc_type(name).is_some() {
			return false;
		}
		if self.is_requested(name) {
			if self.config.types.contains(name) {
				self.used_types.borrow_mut().insert(name.to_string());
			}
			if self.config.structs.contains(name) {
				self.used_structs.borrow_mut().insert(name.to_string());
			}
			if self.config.opaque_structs.contains(name) {
				self.used_opaque_structs.borrow_mut().insert(name.to_string());
			}
			if self.config.unions.contains(name) {
				self.used_unions.borrow_mut().insert(name.to_string());
			}
			true
		} else {
			false
		}
	}

	fn should_keep_const(&self, name: &str) -> bool {
		let is_const = self.config.constants.contains(name);
		let is_const_type = self.config.const_types.contains_key(name);
		if is_const {
			self.used_constants.borrow_mut().insert(name.to_string());
		}
		if is_const_type {
			self.used_constants.borrow_mut().insert(name.to_string());
		}
		is_const || is_const_type
	}

	fn should_keep_function(&self, name: &str) -> bool {
		let is_fn = self.config.functions.contains(name);
		if is_fn {
			self.used_functions.borrow_mut().insert(name.to_string());
		}
		is_fn
	}

	fn should_keep_static(&self, name: &str) -> bool {
		let is_static = self.config.statics.contains(name);
		if is_static {
			self.used_statics.borrow_mut().insert(name.to_string());
		}
		is_static
	}

	fn should_keep_any(&self, name: &str) -> bool {
		if map_mlibc_type(name).is_some() {
			return false;
		}
		self.config.functions.contains(name)
			|| self.config.structs.contains(name)
			|| self.config.unions.contains(name)
			|| self.config.constants.contains(name)
			|| self.config.const_types.contains_key(name)
			|| self.config.types.contains(name)
			|| self.config.statics.contains(name)
			|| self.config.opaque_structs.contains(name)
	}

	fn check_unused(&self, blocked_types: &HashSet<String>) -> Result<(), Box<dyn std::error::Error>> {
		let mut unused = Vec::new();

		let used_fns = self.used_functions.borrow();
		for f in &self.config.functions {
			if !used_fns.contains(f) {
				unused.push(format!("function '{}'", f));
			}
		}

		let used_structs = self.used_structs.borrow();
		for s in &self.config.structs {
			if !used_structs.contains(s) && !blocked_types.contains(s) {
				unused.push(format!("struct '{}'", s));
			}
		}

		let used_unions = self.used_unions.borrow();
		for u in &self.config.unions {
			if !used_unions.contains(u) && !blocked_types.contains(u) {
				unused.push(format!("union '{}'", u));
			}
		}

		let used_consts = self.used_constants.borrow();
		for cn in &self.config.constants {
			if !used_consts.contains(cn) {
				unused.push(format!("constant '{}'", cn));
			}
		}
		for ct in self.config.const_types.keys() {
			if !used_consts.contains(ct) {
				unused.push(format!("constant (typed) '{}'", ct));
			}
		}

		let used_types = self.used_types.borrow();
		for t in &self.config.types {
			if !used_types.contains(t) && !blocked_types.contains(t) {
				unused.push(format!("type '{}'", t));
			}
		}

		let used_statics = self.used_statics.borrow();
		for st in &self.config.statics {
			if !used_statics.contains(st) {
				unused.push(format!("static '{}'", st));
			}
		}

		let used_opaques = self.used_opaque_structs.borrow();
		for op in &self.config.opaque_structs {
			if !used_opaques.contains(op) && !blocked_types.contains(op) {
				unused.push(format!("opaque struct '{}'", op));
			}
		}

		if !unused.is_empty() {
			eprintln!("Error: The following whitelisted items were not found/used in C headers:");
			for item in &unused {
				eprintln!("  - {}", item);
			}
			std::process::exit(1);
		}
		Ok(())
	}
}

/// A syn AST visitor that extracts declared type names (typedefs, structs, unions, enums)
#[derive(Default)]
struct TypeCollector {
	types: HashSet<String>,
}

impl<'ast> Visit<'ast> for TypeCollector {
	fn visit_item_type(&mut self, node: &'ast syn::ItemType) {
		self.types.insert(node.ident.to_string());
		visit::visit_item_type(self, node);
	}

	fn visit_item_struct(&mut self, node: &'ast syn::ItemStruct) {
		self.types.insert(node.ident.to_string());
		visit::visit_item_struct(self, node);
	}

	fn visit_item_union(&mut self, node: &'ast syn::ItemUnion) {
		self.types.insert(node.ident.to_string());
		visit::visit_item_union(self, node);
	}

	fn visit_item_enum(&mut self, node: &'ast syn::ItemEnum) {
		self.types.insert(node.ident.to_string());
		visit::visit_item_enum(self, node);
	}

	fn visit_item_macro(&mut self, node: &'ast syn::ItemMacro) {
		let is_s_macro = node.mac.path.is_ident("s") || node.mac.path.is_ident("s_no_extra_traits");
		if is_s_macro {
			if let Ok(syntax_tree) = syn::parse2::<syn::File>(node.mac.tokens.clone()) {
				self.visit_file(&syntax_tree);
			}
		}
		visit::visit_item_macro(self, node);
	}
}

/// Recursively scans parent mod.rs files up to `src/` to collect already-defined types
fn collect_parent_types(target_dir: &Path) -> Result<HashSet<String>, Box<dyn std::error::Error>> {
	let mut collector = TypeCollector::default();
	let mut current = target_dir.parent();

	while let Some(dir) = current {
		// Look for mod.rs or lib.rs in parent folders
		let candidate_files = [dir.join("mod.rs"), dir.join("lib.rs")];

		for file_path in candidate_files {
			if file_path.is_file() {
				let content = fs::read_to_string(&file_path)?;
				if let Ok(syntax_tree) = syn::parse_file(&content) {
					collector.visit_file(&syntax_tree);
				}
			}
		}

		// Stop climbing once we reach crate root ("src")
		if dir.file_name().and_then(|s| s.to_str()) == Some("src") {
			break;
		}
		current = dir.parent();
	}

	Ok(collector.types)
}

fn find_headers(dir: &Path) -> Result<Vec<PathBuf>, Box<dyn std::error::Error>> {
	let mut headers = Vec::new();
	fn visit(dir: &Path, headers: &mut Vec<PathBuf>) -> Result<(), Box<dyn std::error::Error>> {
		for entry in fs::read_dir(dir)? {
			let entry = entry?;
			let path = entry.path();
			if path.is_dir() {
				visit(&path, headers)?;
			} else if path.extension().and_then(|s| s.to_str()) == Some("h") {
				headers.push(path);
			}
		}
		Ok(())
	}
	visit(dir, &mut headers)?;
	Ok(headers)
}

#[derive(Debug)]
struct LibcStructTagger;

impl ParseCallbacks for LibcStructTagger {
	fn add_attributes(&self, info: &AttributeInfo<'_>) -> Vec<String> {
		if matches!(info.kind, TypeKind::Struct | TypeKind::Union) {
			return vec!["#[doc = \"__LIBC_S_MACRO_TARGET__\"]".into()];
		}
		vec![]
	}
}

fn is_libc_target_and_strip(attrs: &mut Vec<syn::Attribute>) -> bool {
	let mut is_libc_target = false;
	for attr in attrs.iter() {
		if let syn::Meta::NameValue(meta) = &attr.meta {
			if meta.path.is_ident("doc") {
				if let syn::Expr::Lit(expr_lit) = &meta.value {
					if let syn::Lit::Str(lit_str) = &expr_lit.lit {
						if lit_str.value() == "__LIBC_S_MACRO_TARGET__" {
							is_libc_target = true;
							break;
						}
					}
				}
			}
		}
	}

	if is_libc_target {
		attrs.retain(|attr| {
			if let syn::Meta::NameValue(meta) = &attr.meta {
				if meta.path.is_ident("doc") {
					if let syn::Expr::Lit(expr_lit) = &meta.value {
						if let syn::Lit::Str(lit_str) = &expr_lit.lit {
							return lit_str.value() != "__LIBC_S_MACRO_TARGET__";
						}
					}
				}
			}
			true
		});
	}

	is_libc_target
}

fn format_with_rustfmt(code: &str) -> Result<String, Box<dyn std::error::Error>> {
	let mut child = Command::new("rustfmt")
		.arg("--config")
		.arg("struct_lit_single_line=false")
		.stdin(Stdio::piped())
		.stdout(Stdio::piped())
		.spawn()?;

	if let Some(mut stdin) = child.stdin.take() {
		stdin.write_all(code.as_bytes())?;
	}

	let output = child.wait_with_output()?;
	if output.status.success() {
		Ok(String::from_utf8(output.stdout)?)
	} else {
		Err(format!("rustfmt error: {}", String::from_utf8_lossy(&output.stderr)).into())
	}
}

fn strip_derive_attrs(attrs: &mut Vec<syn::Attribute>) {
	attrs.retain(|attr| !attr.meta.path().is_ident("derive"));
}

fn strip_repr_c_attrs(attrs: &mut Vec<syn::Attribute>) {
	attrs.retain(|attr| {
		if attr.meta.path().is_ident("repr") {
			if let syn::Meta::List(meta_list) = &attr.meta {
				let tokens_str = meta_list.tokens.to_string();
				if tokens_str == "C" {
					return false;
				}
			}
		}
		true
	});
}

fn replace_mlibc_types(ty: &mut syn::Type) {
	if let syn::Type::Path(type_path) = ty {
		if type_path.path.leading_colon.is_none() && type_path.path.segments.len() == 1 {
			let ident_str = type_path.path.segments[0].ident.to_string();
			if let Some(mapped_type_str) = map_mlibc_type(&ident_str) {
				if let Ok(new_ty) = syn::parse_str::<syn::Type>(mapped_type_str) {
					*ty = new_ty;
					return;
				}
			}
		}
	}

	match ty {
		syn::Type::Array(type_array) => {
			replace_mlibc_types(&mut type_array.elem);
		}
		syn::Type::Group(type_group) => {
			replace_mlibc_types(&mut type_group.elem);
		}
		syn::Type::Paren(type_paren) => {
			replace_mlibc_types(&mut type_paren.elem);
		}
		syn::Type::Ptr(type_ptr) => {
			replace_mlibc_types(&mut type_ptr.elem);
		}
		syn::Type::Reference(type_reference) => {
			replace_mlibc_types(&mut type_reference.elem);
		}
		syn::Type::Slice(type_slice) => {
			replace_mlibc_types(&mut type_slice.elem);
		}
		syn::Type::Tuple(type_tuple) => {
			for elem in &mut type_tuple.elems {
				replace_mlibc_types(elem);
			}
		}
		syn::Type::Path(type_path) => {
			for segment in &mut type_path.path.segments {
				if let syn::PathArguments::AngleBracketed(args) = &mut segment.arguments {
					for arg in &mut args.args {
						if let syn::GenericArgument::Type(arg_ty) = arg {
							replace_mlibc_types(arg_ty);
						}
					}
				}
			}
		}
		_ => {}
	}
}

fn replace_mlibc_types_in_item(item: &mut syn::Item, whitelist: &Whitelist) -> Result<(), Box<dyn std::error::Error>> {
	match item {
		syn::Item::Struct(struct_item) => {
			for field in &mut struct_item.fields {
				replace_mlibc_types(&mut field.ty);
			}
		}
		syn::Item::Union(union_item) => {
			for field in &mut union_item.fields.named {
				replace_mlibc_types(&mut field.ty);
			}
		}
		syn::Item::Type(type_item) => {
			replace_mlibc_types(&mut type_item.ty);
		}
		syn::Item::Const(const_item) => {
			replace_mlibc_types(&mut const_item.ty);
			let name = const_item.ident.to_string();
			let mut overridden = false;
			if let Some(custom_type_str) = whitelist.config.const_types.get(&name) {
				let new_ty = syn::parse_str::<syn::Type>(custom_type_str)
					.map_err(|e| format!("Failed to parse custom type '{}' for constant '{}': {}", custom_type_str, name, e))?;
				const_item.ty = Box::new(new_ty);
				overridden = true;
			}
			if !overridden {
				if let syn::Type::Path(type_path) = &*const_item.ty {
					if type_path.path.leading_colon.is_none() && type_path.path.segments.len() == 1 {
						let type_name = type_path.path.segments[0].ident.to_string();
						// constants should default to c_int
						if type_name == "u32" {
							if let Ok(new_ty) = syn::parse_str::<syn::Type>("crate::c_int") {
								const_item.ty = Box::new(new_ty);
							}
						}
					}
				}
			}
		}
		syn::Item::Static(static_item) => {
			replace_mlibc_types(&mut static_item.ty);
		}
		syn::Item::ForeignMod(foreign_mod) => {
			for foreign_item in &mut foreign_mod.items {
				match foreign_item {
					syn::ForeignItem::Fn(foreign_fn) => {
						for input in &mut foreign_fn.sig.inputs {
							if let syn::FnArg::Typed(pat_type) = input {
								replace_mlibc_types(&mut pat_type.ty);
							}
						}
						if let syn::ReturnType::Type(_, ret_ty) = &mut foreign_fn.sig.output {
							replace_mlibc_types(ret_ty);
						}
					}
					syn::ForeignItem::Static(foreign_static) => {
						replace_mlibc_types(&mut foreign_static.ty);
					}
					_ => {}
				}
			}
		}
		syn::Item::Impl(impl_item) => {
			replace_mlibc_types(&mut impl_item.self_ty);
			for impl_item_inner in &mut impl_item.items {
				if let syn::ImplItem::Fn(impl_fn) = impl_item_inner {
					for input in &mut impl_fn.sig.inputs {
						if let syn::FnArg::Typed(pat_type) = input {
							replace_mlibc_types(&mut pat_type.ty);
						}
					}
					if let syn::ReturnType::Type(_, ret_ty) = &mut impl_fn.sig.output {
						replace_mlibc_types(ret_ty);
					}
				}
			}
		}
		_ => {}
	}
	Ok(())
}

fn inline_typedef_structs(items: &mut Vec<syn::Item>, whitelist: &Whitelist) -> Result<(), Box<dyn std::error::Error>> {
	let mut target_to_aliases: std::collections::HashMap<String, Vec<String>> = std::collections::HashMap::new();

	for item in items.iter() {
		if let syn::Item::Type(type_item) = item {
			let alias_name = type_item.ident.to_string();
			if let syn::Type::Path(type_path) = &*type_item.ty {
				if type_path.path.leading_colon.is_none() && type_path.path.segments.len() == 1 {
					let target_name = type_path.path.segments[0].ident.to_string();
					let keep_alias = whitelist.should_keep_type(&alias_name) || whitelist.should_keep_struct(&alias_name) || whitelist.should_keep_union(&alias_name);
					let keep_target = whitelist.should_keep_struct(&target_name) || whitelist.should_keep_union(&target_name) || whitelist.should_keep_type(&target_name);

					let target_is_struct_or_union = items.iter().any(|item| {
						match item {
							syn::Item::Struct(s) => s.ident == target_name,
							syn::Item::Union(u) => u.ident == target_name,
							_ => false,
						}
					});

					if keep_alias && !keep_target && target_is_struct_or_union {
						target_to_aliases.entry(target_name.clone()).or_default().push(alias_name.clone());
					}
				}
			}
		}
	}

	let mut alias_to_target = std::collections::HashMap::new();
	let mut target_to_alias = std::collections::HashMap::new();

	for (target_name, aliases) in target_to_aliases {
		if aliases.len() == 1 {
			let alias_name = aliases[0].clone();
			alias_to_target.insert(alias_name.clone(), target_name.clone());
			target_to_alias.insert(target_name, alias_name);
		} else {
			return Err(format!(
				"Struct/union target '{}' has multiple aliases ({:?}); multiple aliases are not supported for inlining.",
				target_name, aliases
			).into());
		}
	}

	if target_to_alias.is_empty() {
		return Ok(());
	}

	fn rename_type_ref(ty: &mut syn::Type, target_to_alias: &std::collections::HashMap<String, String>) {
		if let syn::Type::Path(type_path) = ty {
			if type_path.path.leading_colon.is_none() && type_path.path.segments.len() == 1 {
				let ident_str = type_path.path.segments[0].ident.to_string();
				if let Some(alias_name) = target_to_alias.get(&ident_str) {
					type_path.path.segments[0].ident = syn::Ident::new(alias_name, type_path.path.segments[0].ident.span());
					return;
				}
			}
		}

		match ty {
			syn::Type::Array(type_array) => {
				rename_type_ref(&mut type_array.elem, target_to_alias);
			}
			syn::Type::Group(type_group) => {
				rename_type_ref(&mut type_group.elem, target_to_alias);
			}
			syn::Type::Paren(type_paren) => {
				rename_type_ref(&mut type_paren.elem, target_to_alias);
			}
			syn::Type::Ptr(type_ptr) => {
				rename_type_ref(&mut type_ptr.elem, target_to_alias);
			}
			syn::Type::Reference(type_reference) => {
				rename_type_ref(&mut type_reference.elem, target_to_alias);
			}
			syn::Type::Slice(type_slice) => {
				rename_type_ref(&mut type_slice.elem, target_to_alias);
			}
			syn::Type::Tuple(type_tuple) => {
				for elem in &mut type_tuple.elems {
					rename_type_ref(elem, target_to_alias);
				}
			}
			syn::Type::Path(type_path) => {
				for segment in &mut type_path.path.segments {
					if let syn::PathArguments::AngleBracketed(args) = &mut segment.arguments {
						for arg in &mut args.args {
							if let syn::GenericArgument::Type(arg_ty) = arg {
								rename_type_ref(arg_ty, target_to_alias);
							}
						}
					}
				}
			}
			_ => {}
		}
	}

	for item in items.iter_mut() {
		match item {
			syn::Item::Struct(struct_item) => {
				let current_name = struct_item.ident.to_string();
				if let Some(alias_name) = target_to_alias.get(&current_name) {
					struct_item.ident = syn::Ident::new(alias_name, struct_item.ident.span());
				}
				for field in &mut struct_item.fields {
					rename_type_ref(&mut field.ty, &target_to_alias);
				}
			}
			syn::Item::Union(union_item) => {
				let current_name = union_item.ident.to_string();
				if let Some(alias_name) = target_to_alias.get(&current_name) {
					union_item.ident = syn::Ident::new(alias_name, union_item.ident.span());
				}
				for field in &mut union_item.fields.named {
					rename_type_ref(&mut field.ty, &target_to_alias);
				}
			}
			syn::Item::Type(type_item) => {
				rename_type_ref(&mut type_item.ty, &target_to_alias);
			}
			syn::Item::Const(const_item) => {
				rename_type_ref(&mut const_item.ty, &target_to_alias);
			}
			syn::Item::Static(static_item) => {
				rename_type_ref(&mut static_item.ty, &target_to_alias);
			}
			syn::Item::ForeignMod(foreign_mod) => {
				for foreign_item in &mut foreign_mod.items {
					match foreign_item {
						syn::ForeignItem::Fn(foreign_fn) => {
							for input in &mut foreign_fn.sig.inputs {
								if let syn::FnArg::Typed(pat_type) = input {
									rename_type_ref(&mut pat_type.ty, &target_to_alias);
								}
							}
							if let syn::ReturnType::Type(_, ret_ty) = &mut foreign_fn.sig.output {
								rename_type_ref(ret_ty, &target_to_alias);
							}
						}
						syn::ForeignItem::Static(foreign_static) => {
							rename_type_ref(&mut foreign_static.ty, &target_to_alias);
						}
						_ => {}
					}
				}
			}
			_ => {}
		}
	}

	items.retain(|item| {
		if let syn::Item::Type(type_item) = item {
			let alias_name = type_item.ident.to_string();
			if alias_to_target.contains_key(&alias_name) {
				return false;
			}
		}
		true
	});
	Ok(())
}

fn rename_opaque_fields(struct_item: &mut syn::ItemStruct) {
	for field in &mut struct_item.fields {
		if let Some(ident) = &mut field.ident {
			if ident == "_bindgen_opaque_blob" {
				*ident = syn::Ident::new("opaque", ident.span());
			}
		}
	}
}

fn get_use_tree_names(tree: &syn::UseTree, names: &mut Vec<String>) {
	match tree {
		syn::UseTree::Name(use_name) => {
			names.push(use_name.ident.to_string());
		}
		syn::UseTree::Rename(use_rename) => {
			names.push(use_rename.rename.to_string());
		}
		syn::UseTree::Path(use_path) => {
			get_use_tree_names(&use_path.tree, names);
		}
		syn::UseTree::Group(use_group) => {
			for item in &use_group.items {
				get_use_tree_names(item, names);
			}
		}
		syn::UseTree::Glob(_) => {}
	}
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
	let args = Args::parse();

	// 1. Load whitelist
	let whitelist = Whitelist::load(&args.config)?;

	// 2. Get target directory (parent of mod.rs) and collect parent types
	let target_dir = args.mod_path.parent().ok_or("Invalid mod_path directory")?;
	let blocked_types = collect_parent_types(target_dir)?;
	println!(
		"Discovered {} types in parent modules to blocklist.",
		blocked_types.len()
	);

	// 3. Discover headers from target headers directory
	let mut header_paths = find_headers(&args.headers_dir)?;
	header_paths.sort();

	let mut include_lines = Vec::new();
	for header in header_paths {
		let rel_path = header.strip_prefix(&args.headers_dir)?;
		let rel_str = rel_path.to_string_lossy().replace('\\', "/");
		// Skip internal headers, typically under "bits/" or starting with "_"
		if rel_str.contains("bits/") || rel_str.starts_with('_') || rel_str.contains("/_") {
			continue;
		}
		include_lines.push(format!("#include <{}>", rel_str));
	}

	// 4. Write temp wrapper header
	let mut wrapper_file = tempfile::Builder::new()
		.prefix("libc_parser_wrapper_")
		.suffix(".h")
		.tempfile()?;

	let wrapper_content = include_lines.join("\n");
	wrapper_file.write_all(wrapper_content.as_bytes())?;
	let wrapper_path = wrapper_file.path().to_path_buf();

	// 5. Build bindgen builder
	let mut builder = bindgen::Builder::default()
		.header(wrapper_path.to_str().ok_or("Invalid wrapper path")?)
		.clang_arg(format!("-I{}", args.headers_dir.to_str().ok_or("Invalid headers_dir path")?))
		.clang_arg(format!("--target={}", args.target))
		.clang_arg("-D_GNU_SOURCE")
		.ctypes_prefix("crate")
		.use_core()
		.no_debug(".*")
		.no_default(".*")
		.layout_tests(false)
		.parse_callbacks(Box::new(LibcStructTagger));

	for inc_dir in &args.include_dirs {
		builder = builder.clang_arg(format!("-I{}", inc_dir.to_str().ok_or("Invalid include_dir path")?));
	}

	// 6. Blocklist parent types dynamically
	for type_name in &blocked_types {
		builder = builder.blocklist_type(type_name);
	}

	for opaque_name in &whitelist.config.opaque_structs {
		builder = builder.opaque_type(opaque_name);
	}

	let bindings_res = builder.generate();


	let bindings = bindings_res.map_err(|e| format!("Failed to generate bindings: {:?}", e))?;

	// 7. Parse output into syn AST and merge structs/unions into s! { ... }
	let mut syntax_tree = parse_file(&bindings.to_string())?;
	for item in &mut syntax_tree.items {
		replace_mlibc_types_in_item(item, &whitelist)?;
	}
	inline_typedef_structs(&mut syntax_tree.items, &whitelist)?;

	enum OutputItem {
		Standard(Item),
		SBlock(Vec<Item>),
	}

	let mut transformed_items: Vec<OutputItem> = Vec::new();
	let mut pending_s_items = Vec::new();
	let mut pending_foreign_items = Vec::new();

	let flush_s_macro = |pending: &mut Vec<Item>, target: &mut Vec<OutputItem>| {
		if pending.is_empty() {
			return;
		}
		let items = pending.drain(..).collect();
		target.push(OutputItem::SBlock(items));
	};

	for item in syntax_tree.items {
		let mut is_target = false;
		let mut processed_item = item;

		match &mut processed_item {
			Item::Use(use_item) => {
				let mut names = Vec::new();
				get_use_tree_names(&use_item.tree, &mut names);
				if !names.is_empty() && !names.iter().any(|name| whitelist.should_keep_any(name)) {
					continue;
				}
			}
			Item::ForeignMod(foreign_mod) => {
				const RUST_KEYWORDS: &[&str] = &[
					"as", "break", "const", "continue", "crate", "else", "enum", "extern", "false", "fn",
					"for", "if", "impl", "in", "let", "loop", "match", "mod", "move", "mut", "pub",
					"ref", "return", "self", "Self", "static", "struct", "super", "trait", "true", "type",
					"unsafe", "use", "where", "while", "async", "await", "dyn",
				];

				for mut foreign_item in foreign_mod.items.drain(..) {
					match &mut foreign_item {
						// Formatting: strip leading double underscores from function argument names
						syn::ForeignItem::Fn(foreign_fn) => {
							let name = foreign_fn.sig.ident.to_string();
							if !whitelist.should_keep_function(&name) {
								continue;
							}
							foreign_fn.sig.safety = syn::Safety::Default;
							for input in foreign_fn.sig.inputs.iter_mut() {
								if let syn::FnArg::Typed(pat_type) = input {
									if let syn::Pat::Ident(pat_ident) = &mut *pat_type.pat {
										let name = pat_ident.ident.to_string();
										if name.starts_with("__") && name.len() > 2 {
											let mut new_name = name[2..].to_string();
											if RUST_KEYWORDS.contains(&new_name.as_str()) {
												new_name.push('_');
											}
											pat_ident.ident = syn::Ident::new(&new_name, pat_ident.ident.span());
										}
									}
								}
							}
						}
						syn::ForeignItem::Static(foreign_static) => {
							let name = foreign_static.ident.to_string();
							if !whitelist.should_keep_static(&name) {
								continue;
							}
						}
						_ => {}
					}
					pending_foreign_items.push(foreign_item);
				}
				continue; // Skip pushing the ForeignMod item itself
			}
			Item::Impl(impl_item) => {
				if let syn::Type::Path(type_path) = &*impl_item.self_ty {
					if let Some(segment) = type_path.path.segments.last() {
						let struct_name = segment.ident.to_string();
						if !whitelist.should_keep_struct(&struct_name) && !whitelist.should_keep_union(&struct_name) {
							continue;
						}
					}
				}

				if let Some((trait_path, _)) = &impl_item.trait_ {
					let last_segment = trait_path.segments.iter().last().map(|s| s.ident.to_string());
					if let Some(name) = last_segment {
						if name == "Copy" || name == "Clone" {
							// Discard the generated Copy/Clone impl to avoid duplicates with the s! macro
							continue;
						}
					}
				}
			}
			Item::Struct(struct_item) => {
				let name = struct_item.ident.to_string();
				if !whitelist.should_keep_struct(&name) {
					continue;
				}
				rename_opaque_fields(struct_item);

				if is_libc_target_and_strip(&mut struct_item.attrs) {
					is_target = true;
					strip_derive_attrs(&mut struct_item.attrs);
					strip_repr_c_attrs(&mut struct_item.attrs);
				}
			}
			Item::Union(union_item) => {
				let name = union_item.ident.to_string();
				if !whitelist.should_keep_union(&name) {
					continue;
				}
				if is_libc_target_and_strip(&mut union_item.attrs) {
					is_target = true;
					strip_derive_attrs(&mut union_item.attrs);
					strip_repr_c_attrs(&mut union_item.attrs);
				}
			}
			Item::Type(type_item) => {
				let name = type_item.ident.to_string();
				if let Some(mapped_type_str) = map_mlibc_type(&name) {
					if let Ok(new_ty) = syn::parse_str::<syn::Type>(mapped_type_str) {
						type_item.ty = Box::new(new_ty);
					}
				}
				if !whitelist.should_keep_type(&name) {
					continue;
				}
			}
			Item::Const(const_item) => {
				let name = const_item.ident.to_string();
				if !whitelist.should_keep_const(&name) {
					continue;
				}
			}
			_ => {}
		}

		if is_target {
			pending_s_items.push(processed_item);
		} else {
			flush_s_macro(&mut pending_s_items, &mut transformed_items);
			transformed_items.push(OutputItem::Standard(processed_item));
		}
	}
	flush_s_macro(&mut pending_s_items, &mut transformed_items);

	let mut final_code = String::new();

	let architectures = &whitelist.config.architectures;

	if !architectures.is_empty() {
		final_code.push_str("cfg_if! {\n");
		for (i, arch) in architectures.iter().enumerate() {
			if i == 0 {
				final_code.push_str(&format!("    if #[cfg(target_arch = \"{}\")] {{\n", arch));
			} else {
				final_code.push_str(&format!("    }} else if #[cfg(target_arch = \"{}\")] {{\n", arch));
			}
			final_code.push_str(&format!("        mod {};\n", arch));
			final_code.push_str(&format!("        pub use self::{}::*;\n", arch));
		}
		final_code.push_str("    }\n");
		final_code.push_str("}\n\n");
	}

	let prepend_content = fs::read_to_string(&args.prepend_file)?;
	final_code.push_str(&prepend_content);
	final_code.push_str("\n\n");

	for item in transformed_items {
		match item {
			OutputItem::Standard(standard_item) => {
				let quoted = quote!(#standard_item).to_string();
				final_code.push_str(&quoted);
				final_code.push('\n');
			}
			OutputItem::SBlock(s_items) => {
				if !final_code.is_empty() && !final_code.ends_with("\n\n") {
					final_code.push('\n');
				}
				let mut formatted_parts = Vec::new();
				for item in s_items {
					let item_code = quote!(#item).to_string();
					let formatted = format_with_rustfmt(&item_code)?;
					formatted_parts.push(formatted.trim().to_string());
				}
				let formatted_inner = formatted_parts.join("\n\n");

				let mut indented = String::new();
				for line in formatted_inner.lines() {
					if !line.trim().is_empty() {
						indented.push_str("    ");
					}
					indented.push_str(line);
					indented.push('\n');
				}

				final_code.push_str("s! {\n");
				final_code.push_str(&indented);
				final_code.push_str("}\n\n");
			}
		}
	}

	// Append all foreign items in a single merged extern "C" block
	if !pending_foreign_items.is_empty() {
		let merged_foreign = quote! {
			extern "C" {
				#(#pending_foreign_items)*
			}
		};
		// Parse and format the extern block to look clean
		let parsed_foreign = syn::parse2::<Item>(merged_foreign)?;
		let quoted_foreign = quote!(#parsed_foreign).to_string();
		final_code.push_str(&quoted_foreign);
		final_code.push('\n');
	}

	whitelist.check_unused(&blocked_types)?;

	// 8. Emit formatted output
	let formatted_code = format_with_rustfmt(&final_code)?;

	// Create parent dir of target mod.rs if missing, and write mod.rs
	if let Some(parent) = args.mod_path.parent() {
		fs::create_dir_all(parent)?;
	}
	fs::write(&args.mod_path, formatted_code)?;

	println!("Wrote bindings to {}", args.mod_path.display());
	Ok(())
}

