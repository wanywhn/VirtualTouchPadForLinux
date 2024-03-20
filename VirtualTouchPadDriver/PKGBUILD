# Maintainer: Jia Qingtong <jiaqingtong97@gmail.com>

_pkgbase=virtual-touchpad-driver
pkgname=virtual-touchpad-driver-dkms
pkgver=1
pkgrel=1
pkgdesc="The Virtual Touchpad Driver (DKMS)"
arch=('x86_64', 'aarch64', 'loongarch64')
url="https://github.com/wanywhn/VirtualTouchPadForLinux"
license=('GPL2')
depends=('dkms')
conflicts=("${_pkgbase}")
install=${pkgname}.install
source=("${url}/files/tarball.tar.gz"
        'dkms.conf'
        "${pkgname}.conf"
        )
md5sums=(use 'updpkgsums')

prepare() {
  cd ${_pkgbase}-${pkgver}
}

package() {
  # Copy dkms.conf
  install -Dm644 dkms.conf "${pkgdir}"/usr/src/${_pkgbase}-${pkgver}/dkms.conf

  # Set name and version
  sed -e "s/@_PKGBASE@/${_pkgbase}/" \
      -e "s/@PKGVER@/${pkgver}/" \
      -i "${pkgdir}"/usr/src/${_pkgbase}-${pkgver}/dkms.conf

  # Copy sources (including Makefile)
  cp -r ${_pkgbase}/* "${pkgdir}"/usr/src/${_pkgbase}-${pkgver}/

  # Blacklists conflicting module
  install -Dm644 ${pkgname}.conf "${srcdir}/usr/lib/modprobe.d/${pkgname}.conf"
}
