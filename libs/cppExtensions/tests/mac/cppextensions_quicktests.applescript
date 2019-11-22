property XLib : missing value
property APITestlib : missing value

property title : "cppextensions liblet quick tests" --Name of the test
property targets : {""}
property testCaseID : 0

on PreTest()
	set my APITestlib to XLib's LoadLibrary("APITestLib.scpt")
	APITestlib's initLib(XLib)
	
	return true
end PreTest

on mainTest()
	
	-- specify the test DLL Name containing tests to be executed
	APITestlib's setTestDLLName("cppextensionsTest_mac.dylib")
	
	-- Set the execution mode to whole group
	APITestlib's setTestMode("Test")
	
	APITestlib's executeTests()
	
	APITestlib's clearSpecificGroups()
	
end mainTest

on run
	
	set XLibPath to do shell script "defaults read com.microsoft.macbu.automation XLibPath"
	set my XLib to load script POSIX file XLibPath
	my XLib's ExecuteScript(me)
	
end run