cp orig.img new_mp3.img
dd if=new_mp3.img skip=63 of=fs.img bs=512
debugfs -w -f build_img.txt fs.img
dd if=fs.img of=new_mp3.img seek=63 bs=512
rm fs.img
