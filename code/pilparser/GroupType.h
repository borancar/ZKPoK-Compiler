#include <llvm/DerivedTypes.h>
#include <llvm/LLVMContext.h>
#include <typeinfo>

class GroupT;

/** 
 * Represents numbers from the group Z
 */
class NumberT {

public:

  /** 
   * Constructs a new type of the specified size
   * 
   * @param BitWidth bit size
   */
  NumberT(const uint64_t BitWidth) : width(BitWidth) {};

  /** 
   * Returns the LLVM backing type of this type
   *
   * @return the type that backs this type (an integer of the specified size)
   */
  virtual llvm::Type *getType() const
  { return llvm::IntegerType::get(llvm::getGlobalContext(), width); }

  /** 
   * Compares two types for equality
   * 
   * @param other the type to compare to
   * 
   * @return true if they are both of the same bit size, false otherwise
   */
  virtual bool operator==(const NumberT& other) const
  { return this->width == other.width && other == *this; }

  /** 
   * Gets the bit size of this type
   * 
   * @return bit size
   */
  virtual uint64_t getBitWidth() const
  { return width; }

  /** 
   * Creates the negation of the operand according to the type
   * 
   * @param a operand
   * 
   * @return -a
   */
  virtual llvm::Value *createNeg(const char *id, llvm::Value *a) const;

  /** 
   * Creates the addition of the operands according to the type
   * 
   * @param a first operand
   * @param b second operand
   * 
   * @return a+b
   */
  virtual llvm::Value *createAdd(const char *id, llvm::Value *a, llvm::Value *b) const;

  /** 
   * Creates the subtraction of the operands according to the type
   * 
   * @param a first operand
   * @param b second operand
   * 
   * @return a-b
   */
  virtual llvm::Value *createSub(const char *id, llvm::Value *a, llvm::Value *b) const;

  /** 
   * Creates the multiplication of the operands according to the type
   * 
   * @param a first operand
   * @param b second operand
   * 
   * @return a*b
   */
  virtual llvm::Value *createMul(const char *id, llvm::Value *a, llvm::Value *b) const;
  
  /** 
   * Creates the exponentiation of the operands according to the type
   * 
   * @param a first operand
   * @param b second operand
   * 
   * @return a^b
   */
  virtual llvm::Value *createExp(const char *id, llvm::Value *a, llvm::Value *b) const;


  virtual NumberT *addWithSubFrom(const NumberT *first) const;
  virtual NumberT *addWithSubFrom(const GroupT *first) const;
  
  virtual NumberT *mulWithExpOn(const NumberT *first) const;
  virtual NumberT *mulWithExpOn(const GroupT *first) const;
  
  /** 
   * Returns the inferred type of the addition operation
   * 
   * @param other the type which is added to this type
   * 
   * @return the resulting type
   */  virtual NumberT *operator+(const NumberT &other) const { return other.addWithSubFrom(this); }

  /** 
   * Returns the inferred type of the subtraction operation
   * 
   * @param other the type which is subtracted from this type
   * 
   * @return the resulting type
   */
  virtual NumberT *operator-(const NumberT &other) const { return other.addWithSubFrom(this); }

  /** 
   * Returns the inferred type of the multiplication operation
   * 
   * @param other the type by which this type is multiplied
   * 
   * @return the resulting type
   */
  virtual NumberT *operator*(const NumberT &other) const { return other.mulWithExpOn(this); }

  /** 
   * Returns the inferred type of the exponentation operation
   * 
   * @param other the exponent for this type
   * 
   * @return the resulting type
   */
  virtual NumberT *operator^(const NumberT &other) const { return other.mulWithExpOn(this); }
  
  
protected:
  const uint64_t width;		/**< bit size of type type */
};

/** 
 * Represents numbers from the groups Zmod*(p) and Zmod+(p)
 */
class GroupT : public NumberT {

public:

  /** 
   * Creates a new group type with the specified modulus
   * 
   * @param Modulus modulus of the group
   */
  GroupT(const llvm::APInt &Modulus) : NumberT(Modulus.getBitWidth()), modulus(Modulus) {};

  /** 
   * Compares two type for equality
   * 
   * @param other the type to compare to
   * 
   * @return true if they are the same type, false otherwise
   */
  bool operator==(const NumberT& other) const
  { 
    if(typeid(other) != typeid(GroupT)) return false;

    const GroupT otherGroup = dynamic_cast<const GroupT&>(other);
    return this->width == otherGroup.width && this->modulus == otherGroup.modulus; }

  llvm::Value *createNeg(const char *id, llvm::Value *a) const;
  llvm::Value *createAdd(const char *id, llvm::Value *a, llvm::Value *b) const;
  llvm::Value *createSub(const char *id, llvm::Value *a, llvm::Value *b) const;
  llvm::Value *createMul(const char *id, llvm::Value *a, llvm::Value *b) const;
  llvm::Value *createExp(const char *id, llvm::Value *a, llvm::Value *b) const;

  /** 
   * Returns the modulus of the group type
   * 
   * @return the modulus
   */
  llvm::APInt getModulus() const { return modulus; }

  /** 
   * Returns the modulus wrapped in a ConstanInt which is suitable as
   * an operand
   * 
   * @return ConstantInt representing the modulus
   */
  llvm::ConstantInt *getModulusConstant() const
  { return llvm::ConstantInt::get(llvm::getGlobalContext(), modulus); }

  
private:
  const llvm::APInt modulus;	/**< the modulus of the group type */
};
