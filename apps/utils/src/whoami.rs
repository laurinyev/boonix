use {
    std::{
        process::*,
        ptr::*,
        ffi::*
    }
};

#[repr(C)]
struct Passwd {
    pw_name: *const i8,
    pw_uid: u32,
    pw_gid: u32,
    pw_dir: *const i8,
    pw_shell: *const i8
}

unsafe extern "C" {
    fn getuid() -> u32;
    fn getpwuid(uid: u32) -> *const Passwd;
}

pub fn run(){
    unsafe {
        let uid = getuid();
        let passwd = getpwuid(uid);
       
        if passwd == null() {
            exit(1);
        }

        let uname = CStr::from_ptr((*passwd).pw_name).to_str().unwrap();
    
        println!("{}",uname);
    }
}
