pub fn run(args: Vec<String>) {
    let mut string = "y".to_string();
    
    if args.len() > 1 {
        string = args[1..].join(" ");
    }

    loop {
        println!("{}",string);
    }
}
