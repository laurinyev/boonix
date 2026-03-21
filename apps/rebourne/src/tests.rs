use std::{
    env::*
};

use super::{
    interp::*,
    resolver::*
};

// has to be called every test cuz fck you 
fn test_init() {
    match var("PATH") {
        Ok(v) => {
            set_path(&v)
        },
        Err(..) => {
            let default_path: &str = option_env!("REBOURNE_DEFAULT_PATH").unwrap_or("/usr/bin:/usr/local/bin:/usr/local/sbin");
            set_path(default_path)
        }
    }
}

#[test]
fn basic_echo() {
    test_init();

    assert_eq!(run("echo \"Werx\""), (0, "Werx\n".to_string()));
}
