import urllib.request
import ssl
ctx = ssl.create_default_context()
ctx.check_hostname = False
ctx.verify_mode = ssl.CERT_NONE
urllib.request.urlretrieve('https://github.com/madler/zlib/archive/v1.3.2.tar.gz', 'C:/dev/vcpkg/downloads/madler-zlib-v1.3.2.tar.gz', context=ctx)
print('Python Download Success')
