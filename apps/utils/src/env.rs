use std::env;

pub fn run(args: Vec<String>){
    for (key, value) in env::vars() {
        println!("{}={}", key, value);
    }
}
