cmd_/home/jcorrenti/hello-char-driver/Module.symvers := sed 's/\.ko$$/\.o/' /home/jcorrenti/hello-char-driver/modules.order | scripts/mod/modpost    -o /home/jcorrenti/hello-char-driver/Module.symvers -e -i Module.symvers   -T -