
clean:
	rm $(PROJECT) $(OBJDIR)/*.o *.pem -f


install:
	cp AndServer /usr/bin
	cp andserver.conf.example /etc/andserver.conf

distclean:
	rm /usr/bin/AndServer /etc/andserver.conf


ubuntu-install:
	cp Initscripts/andserver-service-ubuntu /etc/init/andserver-service.conf
	initctl start andserver-service

ubuntu-uninstall:
	rm /etc/init/andserver-service.conf

