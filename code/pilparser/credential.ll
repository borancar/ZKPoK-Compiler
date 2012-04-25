; ModuleID = 'Credential'

%rty.2 = type { i1024, i410, i1024 }

declare i1 @CheckMembership(i1024)

declare i1 @Verify(i1)

define %rty.2 @Round0(i1024 %U) {
entry:
  %v = call i1024 @llvm.random.i1024.i1024(i1024 0)
  %e = call i1024 @llvm.random.i1024.i1024(i1024 0)
  %A = call i1024 @llvm.modexp.i1024.i1024.i1024.i1024(i1024 0, i1024 %v, i1024 0)
  %A1 = call i1024 @llvm.modmul.i1024.i1024.i1024.i1024(i1024 %U, i1024 %A, i1024 0)
  %A2 = call i1024 @llvm.modexp.i1024.i1024.i1024.i1024(i1024 %A1, i1024 -1, i1024 0)
  %A3 = call i1024 @llvm.modmul.i1024.i1024.i1024.i1024(i1024 0, i1024 %A2, i1024 0)
  %A4 = mul i1024 %e, -1
  %A5 = call i1024 @llvm.modexp.i1024.i1024.i1024.i1024(i1024 %A3, i1024 %A4, i1024 0)
  %mrv = insertvalue %rty.2 undef, i1024 %A5, 0
  %mrv6 = insertvalue %rty.2 %mrv, i1024 %e, 1
  %mrv7 = insertvalue %rty.2 %mrv6, i1024 %v, 2
  ret %rty.2 %mrv7
}

declare i1024 @llvm.random.i1024.i1024(i1024) nounwind readnone

declare i1024 @llvm.modexp.i1024.i1024.i1024.i1024(i1024, i1024, i1024) nounwind readnone

declare i1024 @llvm.modmul.i1024.i1024.i1024.i1024(i1024, i1024, i1024) nounwind readnone
