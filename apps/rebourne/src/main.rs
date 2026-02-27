use std::{
    env::*,
};

mod parser;
mod interp;
mod repl;

fn main() {
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
