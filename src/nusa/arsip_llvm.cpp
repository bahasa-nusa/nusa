#include "nusa/arsip_llvm.h"

#include <llvm/Object/Archive.h>
#include <llvm/Object/ArchiveWriter.h>
#include <llvm/Support/Error.h>
#include <llvm/Support/raw_ostream.h>

#include <vector>

extern "C" int bagkang_llvm_buat_arsip(const char *berkas_keluar,
                                       const char **daftar_berkas,
                                       int jumlah_berkas) {
  if (!berkas_keluar || !daftar_berkas || jumlah_berkas <= 0) {
    llvm::errs() << "Argumen pembuatan arsip tidak valid.\n";
    return 1;
  }

  std::vector<llvm::NewArchiveMember> anggota_arsip;
  for (int i = 0; i < jumlah_berkas; ++i) {
    llvm::Expected<llvm::NewArchiveMember> anggota =
        llvm::NewArchiveMember::getFile(daftar_berkas[i], true);
    if (!anggota) {
      llvm::errs() << "Gagal membaca berkas masukan: " << daftar_berkas[i] << " - "
                   << llvm::toString(anggota.takeError()) << "\n";
      return 1;
    }
    anggota_arsip.push_back(std::move(*anggota));
  }

  llvm::object::Archive::Kind jenis_arsip = llvm::object::Archive::K_GNU;
  if (!anggota_arsip.empty()) {
    jenis_arsip = anggota_arsip.front().detectKindFromObject();
  }

  llvm::Error galat = llvm::writeArchive(
      berkas_keluar,
      anggota_arsip,
      llvm::SymtabWritingMode::NormalSymtab,
      jenis_arsip,
      true,
      false,
      nullptr
  );

  if (galat) {
    llvm::errs() << "Gagal menulis berkas arsip: " << berkas_keluar << " - "
                 << llvm::toString(std::move(galat)) << "\n";
    return 1;
  }

  return 0;
}

