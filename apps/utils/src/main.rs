mod whoami; 
mod ls; 
mod yes; 
mod env; 
mod echo; 
mod symlink; 

use {
    std::{
        env::*, path::*, process::*, collections::*
    }
};

fn main() {
    let mut utils: HashMap<&str,Box<dyn Fn(Vec<String>) -> ()>> = HashMap::new();
    
    utils.insert("true", Box::new(|_| {
        exit(0);
    }));

    utils.insert("false", Box::new(|_| {
        exit(1);
    }));
    
    utils.insert("clear", Box::new(|_| {
        print!("\x1B[H\x1B[2J\x1B[3J");
    }));
    
    utils.insert("ls", Box::new(ls::run));
    utils.insert("symlink", Box::new(symlink::run));
    utils.insert("yes", Box::new(yes::run));
    utils.insert("env", Box::new(env::run));
    utils.insert("echo", Box::new(echo::run));
    utils.insert("whoami", Box::new(whoami::run));

    let mut args: Vec<String> = args().collect();
    
    let exe_name = String::from(Path::new(&args[0]).file_name().unwrap().to_str().unwrap());

    if utils.contains_key(exe_name.as_str()) {
        utils[&exe_name.as_str()](args.clone());
    } else {
        if args.len() > 1 && utils.contains_key(args[1].as_str()) {
            args.remove(0);
            utils[&args[0].as_str()](args.clone());
        } else {
            let mut util_names :Vec<_> = utils.keys().into_iter().map(|a| a.to_string()).collect();
            util_names.sort();
            println!("This is boonix utils v{}",env!("CARGO_PKG_VERSION"));
            println!("Available utilities: {}",util_names.join(", "));
        }
    }
}

