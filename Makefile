include $(TOPDIR)/rules.mk

PKG_NAME:=plc
PKG_VERSION:=1.0
PKG_BUILD_DIR:= $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/plc
	SECTION:=base
	CATEGORY:=Utilities
	TITLE:=plc
	DEPENDS:=+libpthread
endef

define Package/plc/description
	If you can't figure out what this program does, you're probably brain-dead and need immediate medical attention. 
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/plc/install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/plc $(1)/bin/
endef

$(eval $(call BuildPackage,plc))