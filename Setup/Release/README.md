VS2013 installer project should be changed as soon as possible, since it does not handle correctly system file installation/removal

To be 100% sure that uninstall procedure is done correctly the advice is:
- Launch CIE Middleware uninstall
- Reboot machine
- Manually delete CIEPKI.dll from C:\Windows\System32 and C:\Windows\SysWow64
- Remove all files from C:\ProgramData\CIEPKI (hidden folder)
