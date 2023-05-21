# Makefile
include $(TOPDIR)/rules.mk

PACKAGE:=udpping
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/$(PACKAGE)
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=Send ICMP simultaneously.
endef

define Package/$(PACKAGE)/description
	Send ICMP signals to multiple devices simultaneously.
endef

define Build/Compile
	$(TARGET_CXX) $(TARGET_CFLAGS) -o $(PKG_BUILD_DIR)/udpping/src common.cpp main.cpp
endef

define Package/$(PACKAGE)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/udpping $(1)/usr/bin/
endef

$(eval $(call BuildPackage,$(PACKAGE)))
