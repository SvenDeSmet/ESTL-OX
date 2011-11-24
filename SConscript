import glob

####
# import the environment
####
Import('env build_dir')

####
# build sources
####
srcs = map(lambda x,bd=build_dir: '%s/%s' % (bd,x), glob.glob("*.cpp"))
objs = env.SharedObject(srcs)

####
# build the plug-in library and set up an alias
####
lib = env.SharedLibrary('%s/ConvolutionFFT' % build_dir,objs)
libInstall = env.Install(env["PLUGINDIR"], lib)
env.Alias('ConvolutionFFT', libInstall)

####
# return the plug-in library
####
Return("libInstall")

