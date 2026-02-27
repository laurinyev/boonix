use std::{
    env::*,
};

mod resolver;
mod parser;
mod interp;
mod repl;

fn main() {
    match var("PATH") {
        Ok(v) => {
            resolver::set_path(&v)
        },
        Err(..) => {
            let default_path: &str = option_env!("REBOURNE_DEFAULT_PATH").unwrap_or("/usr/bin:/usr/local/bin:/usr/local/sbin");
            unsafe { set_var("PATH",default_path) };
            resolver::set_path(default_path)
        }
    }

    let mut args = args();

    if args.len() < 2 {
        repl::run();
    } else if args.len() < 4 {
        let second_arg  = args.nth(1).unwrap();
        if second_arg == "--help" {
            println!("just run the shell lmao, not like I have any options xD");
        } else {
            println!("script: {}", second_arg);
        }
    }
}
