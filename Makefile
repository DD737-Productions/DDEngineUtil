DEBNAME=ddengine
DEBVRS=1.0
DEBDVRS=1
DEBARC=amd64

DEBDIR=$(DEBNAME)_$(DEBVRS)-$(DEBDVRS)_$(DEBARC)
DEBPKG=$(DEBDIR).deb

compile: main.cpp
	g++ main.cpp -o ddengine

deb-delete: 
	if [ -d "$(DEBDIR)" ]; then \
		rm -r $(DEBDIR); \
	fi

deb-compile: compile
	make deb-delete

	mkdir -p $(DEBDIR)/usr/share/ddengine
	mkdir -p $(DEBDIR)/usr/bin
	mkdir -p $(DEBDIR)/DEBIAN

	touch $(DEBDIR)/usr/share/ddengine
	touch $(DEBDIR)/DEBIAN/control

	cp ddengine $(DEBDIR)/usr/share/ddengine
	cp -r Template $(DEBDIR)/usr/share/ddengine/Template
	cp control $(DEBDIR)/DEBIAN/control

	ln -s /usr/share/ddengine/ddengine $(DEBDIR)/usr/bin/ddengine

deb-build: deb-compile
	sudo dpkg-deb --build --root-owner-group $(DEBDIR)
	make deb-delete

deb-install: $(DEBPKG)
	sudo dpkg -i $(DEBPKG)

deb-remove: $(DEBPKG)
	sudo dpkg -P $(DEBNAME)
