use {
    std::{
        env::*,
        process::*
    },
    crate::parser::*
};

fn interp_ast<'a>(node: AstNode<'a>, is_nested: bool) -> String{
    match node {
        AstNode::Root(_) => "".to_string(),
        AstNode::ParseEnd => "".to_string(),
        AstNode::DiscardMe => "".to_string(),
        AstNode::ExternalCommand(_,_) => "".to_string(),
        AstNode::BuiltinCd(n) => {
            let mut dir = interp_ast(*n,true);
            if is_nested {
                return "".to_string();
            }
            if dir == "" {
                match var("HOME") {
                    Ok(v) => {
                        dir = v;
                    },
                    Err(..) => {
                        eprintln!("cd: HOME isn't set") 
                    }
                } 
            }
            match set_current_dir(dir) {
                Ok(..) => unsafe {
                    set_var("PWD",current_dir().unwrap());
                },
                Err(e) => {
                    eprintln!("Couldn't change directory: {e}");
                }
            };
            return "".to_string();
        },
        AstNode::ConstantString(v) => v.to_string(),
        AstNode::BuiltinExit => {
            if !is_nested {
                exit(0)
            } else {
                "".to_string()
            } 
        },
    }
}

pub fn run(cmd: &str) -> u32 {
    let ast = parse(cmd);
   
    println!("{:?}",ast);

    if let AstNode::Root(v) = ast{
        if v.len() == 0 {
            return 1;
        }

        for c in v {
            _ = interp_ast(c,false);
        }
    } else {
        panic!("parse() returned a none-root AST node");
    }

    return 0;
}
