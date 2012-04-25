; ModuleID = 'Prover'

%rty = type { i1024 }
%rty.0 = type { i160 }

@_s_1 = external thread_local global i160
@_r_1 = external thread_local global i160

declare i1 @CheckMembership(i1024)

declare i1 @Verify(i1)

define void @Round0() {
entry:
  ret void
}

define %rty @Round1() {
entry:
  %_r_1 = call i160 @llvm.random.i160.i160(i160 11)
  store i160 %_r_1, i160* @_r_1
  %_r_11 = load i160* @_r_1
  %_t_1 = call i1024 @llvm.modexp.i1024.i1024.i160.i1024(i1024 3, i160 %_r_11, i1024 23)
  %mrv = insertvalue %rty undef, i1024 %_t_1, 0
  ret %rty %mrv
}

declare i160 @llvm.random.i160.i160(i160) nounwind readnone

declare i1024 @llvm.modexp.i1024.i1024.i160.i1024(i1024, i160, i1024) nounwind readnone

define %rty.0 @Round2(i80 %_c) {
entry:
  %_r_1 = load i160* @_r_1
  %_s_1 = call i160 @llvm.modmul.i160.i160.i80.i160(i160 4, i80 %_c, i160 11)
  %_s_11 = call i160 @llvm.modadd.i160.i160.i160.i160(i160 %_r_1, i160 %_s_1, i160 11)
  store i160 %_s_11, i160* @_s_1
  %_s_12 = load i160* @_s_1
  %mrv = insertvalue %rty.0 undef, i160 %_s_12, 0
  ret %rty.0 %mrv
}

declare i160 @llvm.modmul.i160.i160.i80.i160(i160, i80, i160) nounwind readnone

declare i160 @llvm.modadd.i160.i160.i160.i160(i160, i160, i160) nounwind readnone
