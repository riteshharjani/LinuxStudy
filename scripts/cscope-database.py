import os

# to seach for all the bash files in the data base and dump the paths into cscope.files
# Then run cscope -b. 
# this will generate cscope.out files which can be loaded in vim using "cs add cscope.out"
# With this cscope should be running for bash files as well.

of = open("cscope.files", 'w')
SS = "bash"   
for root, dirs, files in os.walk('./'):
    for f in files:
        try:
            filename = os.path.join(root, f)
            ff = open(filename, "r")
            if (SS in ff.readline()):
                of.write(filename)
                of.write("\n")
            else:
                print("{}: not bash file".format(filename))
            ff.close()
        except:
            print("Error for file {}".format(os.path.join(root, f)))
of.close()
