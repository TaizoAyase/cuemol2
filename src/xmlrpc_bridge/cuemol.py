import xmlrpclib

def init(portno, path, cred):
    global proxy, credential
    url = "http://localhost:"+str(portno)+"/"+path;
    proxy = xmlrpclib.ServerProxy(url)
    credential = cred;

def createObj(name):
    global proxy, credential
    id = proxy.createObj(credential, name);
    obj = Wrapper(id);
    return obj;

def getService(name):
    global proxy, credential
    print "getService name=", name;
    id = proxy.getService(credential, name);
    print "getService result id=", id;
    obj = Wrapper(id);
    return obj;

# class CueMol:

#     def __init__(self, portno, path, cred):
#         url = "http://localhost:"+str(portno)+"/"+path;
#         self.proxy = xmlrpclib.ServerProxy(url)
#         self.credential = cred;

#     def createObj(self, name):
#         id = self.proxy.createObj(self.credential, name);
#         obj = Wrapper(id);
#         return obj;

#     def getService(self, name):
#         id = self.proxy.getService(self.credential, name);
#         obj = Wrapper(id);
#         return obj;

# cuemol = CueMol(8080, "RPC2", "XXX")

##############

class MethodObj:

    def __init__(self, obj, name):
        self.obj = obj;
        self.name = name;

    def __call__(self, *args):
        global proxy, credential

        nargs = len(args)
        print "MethodObj ["+ str(self.obj.UID)+ "]."+ self.name+"("+str(nargs)+") called"

        arg2 = []
        for item in args:
            if (isinstance(item, Wrapper)):
                arg2.append({"UID":item.UID});
            else:
                arg2.append(item);

        rval = proxy.callMethod(credential,
                                       self.obj.UID, self.name, arg2);

        if (isinstance(rval, dict)):
            return Wrapper(rval["UID"])
        else:
            return rval

class Wrapper:

    def __init__(self, uid):
        print "Wrapper.__init__ uid=", uid;
        self.__dict__["UID"] = uid
        
    def __del__(self):
        global proxy, credential
        #print "destructing obj: ", self.UID
        proxy.destroyObj(credential, self.UID);

    def __getattr__(self, name):
        global proxy, credential
        #print "getattr (", name, ") called for obj: ", self.UID
        res = proxy.tryGetProp(credential, self.UID, name);
        rcode = res["rcode"];
        if (rcode==1 or rcode==2):
            rval = res["rval"];
            if (isinstance(rval, dict)):
                return Wrapper(rval["UID"])
            else:
                return rval;
        elif (rcode==3):
            # get method called --> return method obj
            return MethodObj(self, name)
        else:
            # Report ERROR
            print "getattr tryGetProp() NG"

#        res = proxy.hasProp(credential, self.UID, name);
#        if (res==1 or res==2):
#            #print "getattr hasProp() OK"
#            rval = proxy.getProp(credential, self.UID, name);
#            if (isinstance(rval, dict)):
#                return Wrapper(rval["UID"])
#            else:
#                return rval;
#        elif (res==3):
#            #print "getattr hasMethod() OK"
#            return MethodObj(self, name)
#        else:
#            # Report ERROR
#            print "getattr hasProp() NG"

    def __setattr__(self, name, value):
        global proxy, credential
        proxy.setProp(credential, self.UID, name, value);
        return;

