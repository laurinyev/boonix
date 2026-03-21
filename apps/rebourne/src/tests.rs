use super::{
    interp::*,
};

#[test]
fn basic_echo() {
    assert_eq!(run("echo \"Werx\""), (0, "Werx\n".to_string()));
}

#[test]
fn semi_echo() {
    assert_eq!(run("echo \"Werx\"; echo \"Other part too!\""), (0, "Werx\nOther part too!\n".to_string()));
}

#[test]
fn multiline_echo() {
    assert_eq!(run("echo \"Werx\"\necho \"Other line too!\""), (0, "Werx\nOther line too!\n".to_string()));
}
#[test]
fn looong_semi() {
    assert_eq!(run("echo \"a\"; echo \"s\"; echo \"d\"; echo \"f\"; echo \"g\"; echo \"h\""), (0, "a\ns\nd\nf\ng\nh\n".to_string()));
}
