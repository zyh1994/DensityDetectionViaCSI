# Makefile
include $(TOPDIR)/rules.mk

PACKAGE:=csi_loop_ping
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/$(PACKAGE)
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=Sending ICMP data in a loop.
	DEPENDS:=+libstdcpp
endef

define Package/$(PACKAGE)/description
	Send ICMP signal to the device in a loop
endef

define Build/Compile
	$(TARGET_CXX) $(TARGET_CFLAGS) -o $(PKG_BUILD_DIR)/csi_loop_ping main.cpp
endef

define Package/$(PACKAGE)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/csi_loop_ping $(1)/usr/bin/
endef

$(eval $(call BuildPackage,$(PACKAGE)))
