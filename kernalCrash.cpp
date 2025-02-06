#include <ntddk.h>

extern "C" {
  void DriverUnload(PDRIVER_OBJECT DriverObject) {
    UNREFERENCED_PARAMETER(DriverObject);
  }
  NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
    UNREFERENCED_PARAMETER(RegistryPath);
    DriverObject->DriverUnload = DriverUnload;

    KeBugCheckEx(0xDEADDEAD, 0, 0, 0 ,0);

    return STATUS_SUCCESS;
  }
}
