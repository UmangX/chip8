use std::fs;

fn main() {
    let rom_path: &str = "/Users/synyster7x/projects/chip8/ibm.ch8";
    println!("hello umang ");
    let rom_file = fs::File::open(rom_path).expect("the file is not working");
}
