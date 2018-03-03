# MinidriverSpy

A simple spyware tool for sniffing user's PIN and altering user's digital signature in PIV smart card on Microsoft Windows.

After compiling the project, you must replace Windows original 'msclmd.dll' with the compiled dll of this project which has the same name.

Please, get a backup copy of your original 'msclmd.dll'.

To test the dll, you must have a PIV smart card like this:

https://pivkey.com/

Or you can use YubiKey too:

https://www.yubico.com/

run a program which use your YubiKey or PIV smart card and when you enter your token's password (PIN), a popup window appears and displays your PIN which is sniffed from dll.
A hacker can use same technique to steal your YubiKey or PIV smart card password.

The simpleset way to test is using CertUtil.exe command in Windows:

certutil.exe -scinfo
