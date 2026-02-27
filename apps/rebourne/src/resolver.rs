
use std::{
    fs::*,
    path::*
};

static mut PATHS : Vec<String> = Vec::new();

#[allow(static_mut_refs)]
pub fn set_path(path: &str) {
    unsafe { PATHS.clear() }; 
   
    let mut paths = path.to_string().split(":").map(|s| s.to_string()).collect();

    unsafe { PATHS.append(&mut paths);}
}

#[allow(static_mut_refs)]
pub fn resolve(util_name: &str) -> Option<String> {
    for p in unsafe { PATHS.clone() } {
        if !exists(&p).unwrap_or(false) {
            continue;
        }
        let i_hate_this_shi = Path::new(&p).join(util_name);
        let util_path = i_hate_this_shi.to_str().unwrap_or("");

        if !exists(&util_path).unwrap_or(false) {
            continue;
        }

        return Some(util_path.to_string()); 
    } 
    return None;
}
