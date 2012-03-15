#include <llvm/DerivedTypes.h>
#include <llvm/LLVMContext.h>

class GroupT;

class NumberT {

public:
 NumberT(const uint64_t BitWidth) : width(BitWidth) {};

  virtual llvm::Type *getType() const
    { return llvm::IntegerType::get(llvm::getGlobalContext(), width); }

  virtual bool operator==(NumberT& other) const
  { return this->width == other.width; }

  virtual uint64_t getBitWidth() const
  { return width; }

  virtual llvm::Value *createNeg(llvm::Value *a) const;
  virtual llvm::Value *createAdd(llvm::Value *a, llvm::Value *b) const;
  virtual llvm::Value *createSub(llvm::Value *a, llvm::Value *b) const;
  virtual llvm::Value *createMul(llvm::Value *a, llvm::Value *b) const;
  virtual llvm::Value *createExp(llvm::Value *a, llvm::Value *b) const;

  virtual NumberT *addWithSubFrom(const NumberT *first) const;
  virtual NumberT *addWithSubFrom(const GroupT *first) const;
  
  virtual NumberT *mulWithExpOn(const NumberT *first) const;
  virtual NumberT *mulWithExpOn(const GroupT *first) const;
  
  virtual NumberT *operator+(const NumberT &other) const { return other.addWithSubFrom(this); }
  virtual NumberT *operator-(const NumberT &other) const { return other.addWithSubFrom(this); }
  virtual NumberT *operator*(const NumberT &other) const { return other.mulWithExpOn(this); }
  virtual NumberT *operator^(const NumberT &other) const { return other.mulWithExpOn(this); }
  
  
protected:
  const uint64_t width;
};

class GroupT : public NumberT {

public:
  GroupT(const llvm::APInt &Modulus) : NumberT(Modulus.getBitWidth()), modulus(Modulus) {};

  bool operator==(NumberT& other) const
  { 
    GroupT otherGroup = dynamic_cast<GroupT&>(other);
    return this->width == otherGroup.width && this->modulus == otherGroup.modulus; }

  llvm::Value *createNeg(llvm::Value *a) const;
  llvm::Value *createAdd(llvm::Value *a, llvm::Value *b) const;
  llvm::Value *createSub(llvm::Value *a, llvm::Value *b) const;
  llvm::Value *createMul(llvm::Value *a, llvm::Value *b) const;
  llvm::Value *createExp(llvm::Value *a, llvm::Value *b) const;

  llvm::APInt getModulus() const { return modulus; }
  llvm::ConstantInt *getModulusConstant() const
  { return llvm::ConstantInt::get(llvm::getGlobalContext(), modulus); }

  
private:
  const llvm::APInt modulus;
};
