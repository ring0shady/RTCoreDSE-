#include <ntddk.h>

#define DRIVER_NAME "Hello"



void HelloUnload(_In_ PDRIVER_OBJECT DriverObject);
NTSTATUS HelloCreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp);


extern "C" NTSTATUS
DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);

	DbgPrintEx(0, 0, "[%s] Driver says hello.\n", DRIVER_NAME);


	DriverObject->DriverUnload = HelloUnload;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = HelloCreateClose;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = HelloCreateClose;


	UNICODE_STRING devName = RTL_CONSTANT_STRING(L"\\Device\\Hello");

	PDEVICE_OBJECT DeviceObject;
	NTSTATUS status = IoCreateDevice(DriverObject, 0, (PUNICODE_STRING)&devName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &DeviceObject);
	if (!NT_SUCCESS(status)) {
		DbgPrintEx(0, 0, "[%s] Driver: Failed to create device (0x%08X)\n", DRIVER_NAME, status);
		return status;
	}

	DeviceObject->Flags |= DO_BUFFERED_IO;

	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\Hello");
	status = IoCreateSymbolicLink((PUNICODE_STRING)&symLink, (PUNICODE_STRING)&devName);
	if (!NT_SUCCESS(status)) {
		DbgPrintEx(0, 0, "[%s] Driver: Failed to create symbolic link (0x%08X)\n", DRIVER_NAME, status);
		IoDeleteDevice(DeviceObject);
		return status;
	}


	DbgPrintEx(0, 0, "[%s] Driver DriverEntry has completed.\n", DRIVER_NAME);

	return STATUS_SUCCESS;
}



void HelloUnload(_In_ PDRIVER_OBJECT DriverObject)
{

	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\Hello");

	IoDeleteSymbolicLink((PUNICODE_STRING)&symLink);
	IoDeleteDevice(DriverObject->DeviceObject);

	DbgPrintEx(0, 0, "[%s] Driver has been Unloaded.\n", DRIVER_NAME);
}


_Use_decl_annotations_
NTSTATUS HelloCreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
	UNREFERENCED_PARAMETER(DeviceObject);

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}




