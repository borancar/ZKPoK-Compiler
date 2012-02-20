#include <llvm/DerivedTypes.h>
#include <llvm/LLVMContext.h>

class NumberT {
public:
 NumberT(const uint64_t BitWidth) : width(BitWidth) {};

  virtual llvm::Type *getType() const
  { return llvm::IntegerType::get(llvm::getGlobalContext(), width); }

  virtual bool operator==(NumberT& other) const
  { return this->width == other.width; }

  virtual uint64_t getBitWidth() const
  { return width; }

protected:
  const uint64_t width;
};

class GroupT : public NumberT {
public:
  GroupT(const llvm::APInt &Modulus) : NumberT(Modulus.getBitWidth()), modulus(Modulus) {};

  virtual bool operator==(NumberT& other) const
  { 
    GroupT otherGroup = dynamic_cast<GroupT&>(other);
    return this->width == otherGroup.width && this->modulus == otherGroup.modulus; }

  llvm::APInt getModulus() const { return modulus; }
  llvm::ConstantInt *getModulusConstant() const
  { return llvm::ConstantInt::get(llvm::getGlobalContext(), modulus); }

private:
  const llvm::APInt modulus;
};
