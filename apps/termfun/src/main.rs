mod symlink; 
mod cowsay;

use {
    std::{
        env::*, path::*, collections::*
    }
};

fn main() {
    let mut utils: HashMap<&str,Box<dyn Fn(Vec<String>) -> ()>> = HashMap::new();
    
    utils.insert("symlink", Box::new(symlink::run));
    utils.insert("cowsay", Box::new(cowsay::run));

    let mut args: Vec<String> = args().collect();
    
    let exe_name = Path::new(&args[0]).file_name().unwrap().to_str().unwrap();

    if utils.contains_key(exe_name) {
        utils[exe_name](args.clone());
    } else {
        if args.len() > 1 && utils.contains_key(args[1].as_str()) {
            args.remove(0);
            utils[&args[0].as_str()](args.clone());
        } else {
            let mut util_names :Vec<_> = utils.keys().into_iter().map(|a| a.to_string()).collect();
            util_names.sort();
            println!("This is boonix terminal fun utils v{}",env!("CARGO_PKG_VERSION"));
            println!("Available utilities: {}",util_names.join(", "));
        }
    }
}

