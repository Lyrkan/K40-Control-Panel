# Allows PlatformIO to upload directly to AsyncElegantOTA
#
# The following environment variable needs to be set (can be done in a .env file):
#
# OTA_UPLOAD_URL=http://<IP>/update

import requests
import hashlib
import os

Import('env')

def upload_cb(source, target, env):
    try:
        from requests_toolbelt import MultipartEncoder, MultipartEncoderMonitor
        from tqdm import tqdm
        from dotenv import load_dotenv
    except ImportError:
        env.Execute("$PYTHONEXE -m pip install requests_toolbelt")
        env.Execute("$PYTHONEXE -m pip install tqdm")
        env.Execute("$PYTHONEXE -m pip install python-dotenv")
        from requests_toolbelt import MultipartEncoder, MultipartEncoderMonitor
        from tqdm import tqdm

    load_dotenv()

    firmware_path = str(source[0])
    upload_url = os.getenv('OTA_UPLOAD_URL')

    print(f"Firmware path: {firmware_path}")
    print(f"Upload URL: {upload_url}")
    exit
    with open(firmware_path, 'rb') as firmware:
        md5 = hashlib.md5(firmware.read()).hexdigest()
        firmware.seek(0)
        encoder = MultipartEncoder(fields={
            'MD5': md5,
            'firmware': ('firmware', firmware, 'application/octet-stream')}
        )

        bar = tqdm(desc='Upload Progress',
              total=encoder.len,
              dynamic_ncols=True,
              unit='B',
              unit_scale=True,
              unit_divisor=1024
              )

        monitor = MultipartEncoderMonitor(encoder, lambda monitor: bar.update(monitor.bytes_read - bar.n))

        response = requests.post(upload_url, data=monitor, headers={'Content-Type': monitor.content_type})
        bar.close()
        print(response,response.text)

env.AddCustomTarget(
    name="OTA",
    dependencies="$BUILD_DIR/${PROGNAME}.bin",
    actions=[
        upload_cb
    ],
    title="OTA Upload",
    description="Upload firmware to ESP32 using ElegantOTA"
)
