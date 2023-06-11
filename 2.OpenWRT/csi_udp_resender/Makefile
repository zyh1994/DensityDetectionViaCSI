# Makefile
include $(TOPDIR)/rules.mk

PACKAGE:=mulping
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/$(PACKAGE)
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=Resend the CSI data by UDP broadcasting.
endef

define Package/$(PACKAGE)/description
	Resend the CSI data by UDP broadcasting.
endef

define Build/Compile
	$(TARGET_CXX) $(TARGET_CFLAGS) -o $(PKG_BUILD_DIR)/csi_udp_resender/ main.cpp csi_fun.cpp
endef

define Package/$(PACKAGE)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/csi_udp_resender $(1)/usr/bin/
endef

$(eval $(call BuildPackage,$(PACKAGE)))
