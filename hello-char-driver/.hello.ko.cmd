cmd_/home/jcorrenti/hello-char-driver/hello.ko := ld -r -EL  -maarch64linux -z norelro --build-id=sha1  -T scripts/module.lds -o /home/jcorrenti/hello-char-driver/hello.ko /home/jcorrenti/hello-char-driver/hello.o /home/jcorrenti/hello-char-driver/hello.mod.o;  true