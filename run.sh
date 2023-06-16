#!/system/bin/sh
# NRAO (Non root android optimizer) runner
# Author: Looper (iamlooper @ github)

BASEDIR="$(dirname "$0")"

# Check for shell permissions and exit if not granted.
[ "$(id -u)" -ne 2000 ] && {
  echo "[Error] No shell permissions. Exiting."
  exit 1
}

# Setup binaries for execution.
setup_binaries() {
  [ -e "$BASEDIR/libs" ] && {
    cp -f "$BASEDIR/libs/$1/nrao" '/data/local/tmp'
    cp -f "$BASEDIR/libs/$1/vmtouch" '/data/local/tmp'
    [ "$?" -ne 0 ] && {
      echo "[Error] Unsupported ABI. Exiting."
      exit 1
    } 
    chmod 0755 '/data/local/tmp/nrao'
    chmod 0755 '/data/local/tmp/vmtouch'
  } || {
    echo "[Error] Binaries not found. Exiting."
    exit 1
  }
}

# Run NRAO.
run_nrao() {
  # Prepare binaries.
  setup_binaries "$(getprop ro.product.cpu.abi)"
  # Execute NRAO.
  /data/local/tmp/nrao 1>'/storage/emulated/0/Android/nrao.txt' 2>'/data/local/tmp/nrao_error.txt' && cp -f '/data/local/tmp/nrao_error.txt' '/storage/emulated/0/Android' &
  # Show a message.
  echo "[Info] NRAO is executed in background. Check Internal Storage / Android / nrao.txt for more information."
  echo "[Info] Error log (nrao_error.txt) will appear in Internal Storage / Android after NRAO is completely executed."
  # Exit as successful.
  exit 0
}

# Main
run_nrao