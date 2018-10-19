
clean:
	rm $(PROJECT) $(OBJDIR)/*.o *.pem -f


install:
	cp virtual_touchpad_server /usr/bin
	cp virtual_touchpad_server.conf.example /etc/virtual_touchpad_server.conf
	cp Initscripts/virtual-touchpad-services-deepin /etc/init.d/

uninstall:
	rm /usr/bin/virtual_touchpad_server /etc/virtual_touchpad_server.conf


