import gdb
import math as m

class fpointers(gdb.Command):

        def __init__(self):
                super(fpointers,self).__init__("fpointers",gdb.COMMAND_DATA)

        def invoke(self,arg,from_tty):
                args = gdb.string_to_argv(arg)
                if len(args)!=2:
                        print("Error\n")
                else:
                        adress = args[0]
                        plage = int(m.floor(int(args[1])/8))
                        print("m = "+str(plage))
                command = "x/"+str(plage)+"xg "+adress
                result = gdb.execute(command)
                print(result)

fpointers()
