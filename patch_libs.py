Import("env")
import os

# PATCH 1: Disable ezTime EEPROM cache
def patch_eztime(source, target, env):
    path = os.path.join(env["PROJECT_LIBDEPS_DIR"], env["PIOENV"], "ezTime", "src", "ezTime.h")
    
    if not os.path.exists(path):
        print(">> ezTime not found, skipping patch")
        return
        
    with open(path, "r") as f:
        content = f.read()
        
    # Comment out the macro to prevent hardware stalls on SAMD21 boards
    if "#define EZTIME_CACHE_EEPROM" in content and "// #define EZTIME_CACHE_EEPROM" not in content:
        content = content.replace("#define EZTIME_CACHE_EEPROM", "// #define EZTIME_CACHE_EEPROM")
        with open(path, "w") as f:
            f.write(content)
        print(">> Patched ezTime: EZTIME_CACHE_EEPROM disabled")


# PATCH 2: Fix MKR IoT Carrier 'NONE' enum collision
# Reason: The generic 'NONE' enum leaks into the global namespace causing ODR compilation errors. 
# Renaming it to 'SHANE_NONE' resolves conflicts with other libraries.
def patch_carrier(source, target, env):
    path = os.path.join(env["PROJECT_LIBDEPS_DIR"], env["PIOENV"], "Arduino_MKRIoTCarrier", "src", "Arduino_MKRIoTCarrier.h")
    
    if not os.path.exists(path):
        print(">> Arduino_MKRIoTCarrier not found, skipping patch")
        return
        
    with open(path, "r") as f:
        content = f.read()
        
    # Apply patch if not already present
    if "SHANE_NONE" not in content:
        import re
        # Exact regex match to replace 'NONE = -1'
        content = re.sub(r'\bNONE\s*=\s*-1,', 'SHANE_NONE = -1,', content)
        
        with open(path, "w") as f:
            f.write(content)
        print(">> Patched Arduino_MKRIoTCarrier: NONE -> SHANE_NONE")
    else:
        print(">> Arduino_MKRIoTCarrier already patched")


# EXECUTION TRIGGERS

# 1. Run immediately on script load
patch_eztime(None, None, env)
patch_carrier(None, None, env)

# 2. Hook to run right before C++ compilation (handles mid-build fresh library downloads)
env.AddPreAction("compile", lambda source, target, env: [patch_eztime(source, target, env), patch_carrier(source, target, env)])