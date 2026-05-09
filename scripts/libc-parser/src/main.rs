use syn::{visit::Visit, File, ForeignItemFn, ItemConst, ItemEnum, ItemFn, ItemMod, ItemStruct, ItemType, ItemUnion, Visibility};
use std::{env, fs, process};

#[derive(Default)]
struct Collector {
	module_stack: Vec<String>,
	pub_consts: Vec<String>,
	pub_enums: Vec<String>,
	pub_fns: Vec<String>,
	pub_structs: Vec<String>,
	pub_unions: Vec<String>,
	pub_types: Vec<String>,
}

impl Collector {
	fn ignore_mod(&self) -> bool {
		if self.module_stack.len() > 2 || self.module_stack.is_empty() {
			return true;
		}

		if self.module_stack[0] != "unix" {
			return true;
		}

		if self.module_stack.len() == 2 && self.module_stack[1] != "linux_like" {
			return true;
		}

		false
	}
}

impl<'ast> Visit<'ast> for Collector {
	fn visit_item_mod(&mut self, i: &'ast ItemMod) {
		if let Some((_, items)) = &i.content {
			self.module_stack.push(i.ident.to_string());
			for item in items {
				self.visit_item(item);
			}
			self.module_stack.pop();
		}
	}

	fn visit_item_const(&mut self, node: &'ast ItemConst) {
		if self.ignore_mod() {
			return;
		}

		if matches!(node.vis, Visibility::Public(_)) {
			self.pub_consts.push(node.ident.to_string());
		}

		// Continue visiting nested items (if any)
		syn::visit::visit_item_const(self, node);
	}

	fn visit_item_enum(&mut self, node: &'ast ItemEnum) {
		if self.ignore_mod() {
			return;
		}

		if matches!(node.vis, Visibility::Public(_)) {
			self.pub_enums.push(node.ident.to_string());
		}

		// Continue walking the tree if needed
		syn::visit::visit_item_enum(self, node);
	}

	fn visit_item_fn(&mut self, node: &'ast ItemFn) {
		if self.ignore_mod() {
			return;
		}

		if matches!(node.vis, Visibility::Public(_)) {
			self.pub_fns.push(node.sig.ident.to_string());
		}

		// Continue walking the tree if needed
		syn::visit::visit_item_fn(self, node);
	}

	fn visit_foreign_item_fn(&mut self, node: &'ast ForeignItemFn) {
		if self.ignore_mod() {
			return;
		}

		if matches!(node.vis, Visibility::Public(_)) {
			self.pub_fns.push(node.sig.ident.to_string());
		}

		// Continue walking the tree if needed
		syn::visit::visit_foreign_item_fn(self, node);
	}

	fn visit_item_struct(&mut self, node: &'ast ItemStruct) {
		if self.ignore_mod() {
			return;
		}

		if matches!(node.vis, Visibility::Public(_)) {
			self.pub_structs.push(node.ident.to_string());
		}

		// Continue walking the tree if needed
		syn::visit::visit_item_struct(self, node);
	}

	fn visit_item_union(&mut self, node: &'ast ItemUnion) {
		if self.ignore_mod() {
			return;
		}

		if matches!(node.vis, Visibility::Public(_)) {
			self.pub_unions.push(node.ident.to_string());
		}

		// Continue walking the tree if needed
		syn::visit::visit_item_union(self, node);
	}

	fn visit_item_type(&mut self, node: &'ast ItemType) {
		if self.ignore_mod() {
			return;
		}

		if matches!(node.vis, Visibility::Public(_)) {
			self.pub_types.push(node.ident.to_string());
		}

		syn::visit::visit_item_type(self, node);
	}
}

fn process_file(file_path: &str) -> Result<Collector, String> {
	let content = fs::read_to_string(file_path)
		.map_err(|e| format!("Failed to read '{}': {}", file_path, e))?;

	let syntax: File = syn::parse_file(&content)
		.map_err(|e| format!("Failed to parse '{}': {}", file_path, e))?;

	let mut visitor = Collector::default();
	visitor.visit_file(&syntax);

	Ok(visitor)
}

fn main() {
	let args: Vec<String> = env::args().skip(1).collect();

	if args.is_empty() {
		eprintln!("Usage: generate-rust-libc-ignorelist <file1.rs> <file2.rs> ...");
		process::exit(1);
	}

	let mut pub_consts: Vec<String> = Vec::new();
	let mut pub_enums: Vec<String> = Vec::new();
	let mut pub_fns: Vec<String> = Vec::new();
	let mut pub_structs: Vec<String> = Vec::new();
	let mut pub_unions: Vec<String> = Vec::new();
	let mut pub_types: Vec<String> = Vec::new();

	for file_path in &args {
		match process_file(file_path) {
			Ok(v) => {
				pub_consts.extend(v.pub_consts);
				pub_enums.extend(v.pub_enums);
				pub_fns.extend(v.pub_fns);
				pub_structs.extend(v.pub_structs);
				pub_unions.extend(v.pub_unions);
				pub_types.extend(v.pub_types);
			}
			Err(e) => {
				eprintln!("Error parsing {}: {}", file_path, e);
			}
		}
	}

	println!("ignored_macros:");
	for c in &pub_consts {
		println!("  - \"{}\"", c);
	}
	for c in &pub_fns {
		println!("  - \"{}\"", c);
	}

	println!("ignored_structs:");
	for c in &pub_structs {
		println!("  - \"{}\"", c);
	}
	for c in &pub_enums {
		println!("  - \"{}\"", c);
	}
	for c in &pub_unions {
		println!("  - \"{}\"", c);
	}

	println!("ignored_unions:");
	for c in &pub_unions {
		println!("  - \"{}\"", c);
	}
	for c in &pub_structs {
		println!("  - \"{}\"", c);
	}

	println!("ignored_types:");
	for c in &pub_types {
		println!("  - \"{}\"", c);
	}
	for c in &pub_structs {
		println!("  - \"{}\"", c);
	}
	for c in &pub_enums {
		println!("  - \"{}\"", c);
	}
	for c in &pub_unions {
		println!("  - \"{}\"", c);
	}

	println!("ignored_functions:");
	for c in &pub_fns {
		println!("  - \"{}\"", c);
	}
}
