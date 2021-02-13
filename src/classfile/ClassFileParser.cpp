#include "ClassFileParser.hpp"

#include "../JVM.hpp"
#include "../SymbolTable.hpp"


#define JAVA_CLASSFILE_MAGIC              0xCAFEBABE
#define JAVA_MIN_SUPPORTED_VERSION        45
#define JAVA_MAX_SUPPORTED_VERSION        52
#define JAVA_MAX_SUPPORTED_MINOR_VERSION  0

// Used for backward compatibility reasons:
// - to check for javac bug fixes that happened after 1.5
// - also used as the max version when running in jdk6
#define JAVA_6_VERSION                    50

// Used for backward compatibility reasons:
// - to check NameAndType_info signatures more aggressively
#define JAVA_7_VERSION                    51

// Extension method support.
#define JAVA_8_VERSION                    52


namespace CCW::Tula {

    static constexpr const char *ATTRIBUTE_SourceFile = "SourceFile";
    static constexpr const char *ATTRIBUTE_InnerClasses = "InnerClasses";
    static constexpr const char *ATTRIBUTE_EnclosingMethod = "EnclosingMethod";
    static constexpr const char *ATTRIBUTE_SourceDebugExtension = "SourceDebugExtension";
    static constexpr const char *ATTRIBUTE_BootstrapMethods = "BootstrapMethods";
    static constexpr const char *ATTRIBUTE_ConstantValue = "ConstantValue";
    static constexpr const char *ATTRIBUTE_Code = "Code";
    static constexpr const char *ATTRIBUTE_Exceptions = "Exceptions";
    static constexpr const char *ATTRIBUTE_RuntimeVisibleParameterAnnotations = "RuntimeVisibleParameterAnnotations";
    static constexpr const char *ATTRIBUTE_AnnotationDefault = "AnnotationDefault";
    static constexpr const char *ATTRIBUTE_MethodParameters = "MethodParameters";
    static constexpr const char *ATTRIBUTE_Synthetic = "Synthetic";
    static constexpr const char *ATTRIBUTE_Deprecated = "Deprecated";
    static constexpr const char *ATTRIBUTE_Signature = "Signature";
    static constexpr const char *ATTRIBUTE_RuntimeVisibleAnnotations = "RuntimeVisibleAnnotations";
    static constexpr const char *ATTRIBUTE_RuntimeInvisibleAnnotations = "RuntimeInvisibleAnnotations";
    static constexpr const char *ATTRIBUTE_LineNumberTable = "LineNumberTable";
    static constexpr const char *ATTRIBUTE_LocalVariableTable = "LocalVariableTable";
    static constexpr const char *ATTRIBUTE_LocalVariableTypeTable = "LocalVariableTypeTable";
    static constexpr const char *ATTRIBUTE_StackMapTable = "StackMapTable";
    static constexpr const char *ATTRIBUTE_RuntimeVisibleTypeAnnotations = "RuntimeVisibleTypeAnnotations";
    static constexpr const char *ATTRIBUTE_RuntimeInvisibleTypeAnnotations = "RuntimeInvisibleTypeAnnotations";


    std::string printf(const char *fmt, ...) {
        va_list args;
        va_start(args, fmt);
        char buf[32];
        size_t n = std::vsnprintf(buf, sizeof(buf), fmt, args);
        va_end(args);

        // Static buffer large enough?
        if (n < sizeof(buf)) {
            return {buf, n};
        }

        // Static buffer too small
        std::string s(n + 1, 0);
        va_start(args, fmt);
        std::vsnprintf(const_cast<char *>(s.data()), s.size(), fmt, args);
        va_end(args);

        return s;
    }


    Klass::Ptr ClassFileParser::parse(const uint8_t *data, uint32_t len) noexcept(false) {
        ClassFileParser parser(data, len);
        return parser.parse();
    }

    ClassFileParser::ClassFileParser(const uint8_t *data, uint32_t len) :
        reader(data, len), cp(nullptr) {
    }

    void ClassFileParser::throwParseException(const char *fmt, ...) {

        va_list args;
        va_start(args, fmt);
        char buf[32];
        size_t n = std::vsnprintf(buf, sizeof(buf), fmt, args);
        va_end(args);

        // Static buffer large enough?
        if (n < sizeof(buf)) {
            throw ClassFormatError({buf, n});
        }

        // Static buffer too small
        std::string s(n + 1, 0);
        va_start(args, fmt);
        std::vsnprintf(const_cast<char *>(s.data()), s.size(), fmt, args);
        va_end(args);

        throw ClassFormatError(s);
    }

    bool ClassFileParser::isValidCpIndex(uint16_t index) {
        return index > 0 && index < cp->getSize();
    }

    bool isValidDescriptor(const SymbolPtr &descriptor) {
        // TODO validate descriptor
        return true;
    }

#define throwValidExceptionAssert(cond, ...) do { \
        if (!(cond)) { \
            throwParseException(__VA_ARGS__); \
        } \
    } while(0)

    Klass::Ptr ClassFileParser::parse() noexcept(false) {
        auto magic = reader.readU32();
        if (magic != JAVA_CLASSFILE_MAGIC) {
            throwParseException("Invalid Class file magic %u.", magic);
        }

        reader.ensure(4);
        this->minorVersion = reader.readU16Unchecked();
        this->majorVersion = reader.readU16Unchecked();

        parseConstantPool();
        CCW_ASSERT(cp != nullptr);

        reader.ensure(8);
        accessFlags = static_cast<ClassAccessFlags >(reader.readU16Unchecked());
        if (accessFlags & ClassAccessFlags::Interface) {
            throwValidExceptionAssert(accessFlags & ClassAccessFlags::Abstract, "Interface must be abstract.");
            throwValidExceptionAssert(!(accessFlags & ClassAccessFlags::Final), "Interface must be non-final.");
            throwValidExceptionAssert(!(accessFlags & ClassAccessFlags::Super), "Interface must be non-super.");
            throwValidExceptionAssert(!(accessFlags & ClassAccessFlags::Enum), "Interface must be non-enum.");
        } else {
            throwValidExceptionAssert(!(accessFlags & ClassAccessFlags::Annotation), "Class is not annotation");
            throwValidExceptionAssert(
                !(accessFlags & ClassAccessFlags::Final && accessFlags & ClassAccessFlags::Abstract),
                "Final Class can not be abstract.");
        }

        if (accessFlags & ClassAccessFlags::Annotation) {
            throwValidExceptionAssert(accessFlags & ClassAccessFlags::Interface, "Annotation must be interface.");
        }

        auto thisClassIndex = reader.readU16Unchecked();
        throwValidExceptionAssert(isValidCpIndex(thisClassIndex) && cp->getTagAt(thisClassIndex).isUnresolvedClass(),
                                  "Invalid this class index at %d", thisClassIndex);
        auto thisClassName = cp->getClassAt(thisClassIndex);
        auto superClassIndex = reader.readU16Unchecked();
        throwValidExceptionAssert(superClassIndex == 0 || isValidCpIndex(superClassIndex) &&
                                                          cp->getTagAt(
                                                              superClassIndex).isClassOrUnresolvedClass(),
                                  "Invalid super class index at %d", superClassIndex);

        // TODO resolved super class

        parseInterfaces();

        parseFields();

        return nullptr;
    }

    void ClassFileParser::parseFields() noexcept(false) {
        auto fieldCount = reader.readU16();
        auto isInterface = accessFlags & ClassAccessFlags::Interface;
        for (int i = 0; i < fieldCount; ++i) {
            reader.ensure(8);
            auto fieldAccessFlags = static_cast<FieldAccessFlags>(reader.readU16Unchecked());
            if (isInterface) {
                throwValidExceptionAssert(
                    (fieldAccessFlags & FieldAccessFlags::Public)
                    && (fieldAccessFlags & FieldAccessFlags::Final)
                    && (fieldAccessFlags & FieldAccessFlags::Static),
                    "invalid field access flags %d in interface", fieldAccessFlags);
            } else {
                if (fieldAccessFlags & FieldAccessFlags::Public) {
                    throwValidExceptionAssert(
                        !(fieldAccessFlags & FieldAccessFlags::Protected ||
                          fieldAccessFlags & FieldAccessFlags::Private),
                        "invalid field access flags %d", fieldAccessFlags);
                } else if (fieldAccessFlags & FieldAccessFlags::Private) {
                    throwValidExceptionAssert(
                        !(fieldAccessFlags & FieldAccessFlags::Protected ||
                          fieldAccessFlags & FieldAccessFlags::Public),
                        "invalid field access flags %d", fieldAccessFlags);
                } else if (fieldAccessFlags & FieldAccessFlags::Protected) {
                    throwValidExceptionAssert(
                        !(fieldAccessFlags & FieldAccessFlags::Private || fieldAccessFlags & FieldAccessFlags::Public),
                        "invalid field access flags %d", fieldAccessFlags);
                }

                throwValidExceptionAssert(
                    !(fieldAccessFlags & FieldAccessFlags::Final || fieldAccessFlags & FieldAccessFlags::Volatile),
                    "invalid field access flags %d", fieldAccessFlags);
            }

            auto nameIndex = reader.readU16Unchecked();
            throwValidExceptionAssert(isValidCpIndex(nameIndex)
                                      && cp->getTagAt(nameIndex).isUtf8(),
                                      "Invalid field name index at %d", nameIndex);

            auto descriptorIndex = reader.readU16Unchecked();
            throwValidExceptionAssert(isValidCpIndex(descriptorIndex)
                                      && cp->getTagAt(descriptorIndex).isUtf8()
                                      && isValidDescriptor(cp->getSymbolAt(descriptorIndex)),
                                      "Invalid field descriptor index at %d", descriptorIndex);

            parseFieldAttributes(fieldAccessFlags);

            
        }
    }

    void ClassFileParser::parseFieldAttributes(FieldAccessFlags flags) {

        std::optional<uint16_t> constValueIndex;
        std::optional<SymbolPtr> signature;
        bool synthetic = false;
        bool deprecated = false;
        auto attributeCount = reader.readU16Unchecked();
        for (int i = 0; i < attributeCount; ++i) {
            reader.ensure(6);
            auto attrNameIndex = reader.readU16Unchecked();
            throwValidExceptionAssert(isValidCpIndex(attrNameIndex)
                                      && cp->getTagAt(attrNameIndex).isUtf8(),
                                      "Invalid field attribute name index at %d", attrNameIndex);

            uint32_t len = reader.readU32Unchecked();
            reader.ensure(len);

            const auto &attrName = cp->getSymbolAt(attrNameIndex);
            if (attrName->equals(ATTRIBUTE_ConstantValue)) {
                if (flags & FieldAccessFlags::Static) {
                    throwValidExceptionAssert(len == 2, "Invalid constant value attr length %d", len);
                    auto cvIndex = reader.readU16Unchecked();
                    throwValidExceptionAssert(
                        isValidCpIndex(cvIndex)
                        && cp->getTagAt(cvIndex).isConstantValueType(), "Invalid constant value index %d", cvIndex);
                    constValueIndex = cvIndex;
                } else {
                    reader.skip(len);
                }
            } else if (attrName->equals(ATTRIBUTE_Synthetic)) {
                throwValidExceptionAssert(len == 0, "Invalid synthetic attr len");
                synthetic = true;
            } else if (attrName->equals(ATTRIBUTE_Deprecated)) {
                throwValidExceptionAssert(len == 0, "Invalid deprecated attr len");
                deprecated = true;
            } else if (attrName->equals(ATTRIBUTE_Signature)) {
                signature = parseSignatureAttribute(len);
            } else if (attrName->equals(ATTRIBUTE_RuntimeVisibleAnnotations)) {
                /* auto annotations = */ parseAnnotations();
            } else if (attrName->equals(ATTRIBUTE_RuntimeInvisibleAnnotations)) {
                /* auto annotations = */ parseAnnotations();
            } else if (attrName->equals(ATTRIBUTE_RuntimeVisibleTypeAnnotations)) {
                // /* auto typeAnnotations = */ parseTypeAnnotations();
                reader.skip(len);
            } else if (attrName->equals(ATTRIBUTE_RuntimeInvisibleTypeAnnotations)) {
                // /* auto typeAnnotations = */ parseTypeAnnotations();
                reader.skip(len);
            } else {
                reader.skip(len);
            }

        }
    }

    void ClassFileParser::parseAnnotations() noexcept(false) {
        reader.ensure(2);
        auto annotationCount = reader.readU16Unchecked();
        for (int j = 0; j < annotationCount; ++j) {
            parseAnnotation();
        }
    }

    void ClassFileParser::parseAnnotation() noexcept(false) {
        reader.ensure(4);
        auto typeIndex = reader.readU16Unchecked();
        throwValidExceptionAssert(isValidCpIndex(typeIndex)
                                  && cp->getTagAt(typeIndex).isUtf8(),
                                  "Invalid runtime visible annotation type index at %d", typeIndex);
        auto elementValuePairCount = reader.readU16Unchecked();
        for (int k = 0; k < elementValuePairCount; ++k) {
            /*auto elementValue = */ parseElementValue();
        }
    }

    void ClassFileParser::parseElementValue() noexcept(false) {
        reader.ensure(3);
        auto elementNameIndex = reader.readU16Unchecked();
        throwValidExceptionAssert(isValidCpIndex(elementNameIndex)
                                  && cp->getTagAt(elementNameIndex).isUtf8(),
                                  "Invalid element name index at %d", elementNameIndex);
        auto tagValue = reader.readU8Unchecked();
        auto tag = static_cast<ElementValueTag>(tagValue);
        switch (tag) {
            case ElementValueTag::Byte: {
                auto cIndex = reader.readU16();
                throwValidExceptionAssert(isValidCpIndex(cIndex)
                                          && cp->getTagAt(cIndex).isInteger(),
                                          "Invalid const value index at %d", cIndex);
                int32_t value = cp->getIntegerAt(cIndex);
                break;
            }
            case ElementValueTag::Char: {
                auto cIndex = reader.readU16();
                throwValidExceptionAssert(isValidCpIndex(cIndex)
                                          && cp->getTagAt(cIndex).isInteger(),
                                          "Invalid const value index at %d", cIndex);
                auto value = cp->getIntegerAt(cIndex);
                break;
            }
            case ElementValueTag::Double: {
                auto cIndex = reader.readU16();
                throwValidExceptionAssert(isValidCpIndex(cIndex)
                                          && cp->getTagAt(cIndex).isDouble(),
                                          "Invalid const value index at %d", cIndex);
                auto value = cp->getDoubleAt(cIndex);
                break;
            }
            case ElementValueTag::Float: {
                auto cIndex = reader.readU16();
                throwValidExceptionAssert(isValidCpIndex(cIndex)
                                          && cp->getTagAt(cIndex).isFloat(),
                                          "Invalid const value index at %d", cIndex);
                auto value = cp->getFloatAt(cIndex);
                break;
            }
            case ElementValueTag::Int: {
                auto cIndex = reader.readU16();
                throwValidExceptionAssert(isValidCpIndex(cIndex)
                                          && cp->getTagAt(cIndex).isInteger(),
                                          "Invalid const value index at %d", cIndex);
                auto value = cp->getIntegerAt(cIndex);
                break;
            }
            case ElementValueTag::Long: {
                auto cIndex = reader.readU16();
                throwValidExceptionAssert(isValidCpIndex(cIndex)
                                          && cp->getTagAt(cIndex).isLong(),
                                          "Invalid const value index at %d", cIndex);
                auto value = cp->getLongAt(cIndex);
                break;
            }
            case ElementValueTag::Short: {
                auto cIndex = reader.readU16();
                throwValidExceptionAssert(isValidCpIndex(cIndex)
                                          && cp->getTagAt(cIndex).isInteger(),
                                          "Invalid const value index at %d", cIndex);
                auto value = cp->getIntegerAt(cIndex);
                break;
            }
            case ElementValueTag::Boolean: {
                auto cIndex = reader.readU16();
                throwValidExceptionAssert(isValidCpIndex(cIndex)
                                          && cp->getTagAt(cIndex).isInteger(),
                                          "Invalid const value index at %d", cIndex);
                auto value = cp->getIntegerAt(cIndex);
                break;
            }
            case ElementValueTag::String: {
                auto cIndex = reader.readU16();
                throwValidExceptionAssert(isValidCpIndex(cIndex)
                                          && cp->getTagAt(cIndex).isUtf8(),
                                          "Invalid const value index at %d", cIndex);
                auto value = cp->getSymbolAt(cIndex);
                break;
            }
            case ElementValueTag::EnumType: {
                reader.ensure(4);
                auto typeNameIndex = reader.readU16Unchecked();
                auto constNameIndex = reader.readU16Unchecked();
                throwValidExceptionAssert(
                    isValidCpIndex(typeNameIndex) && cp->getTagAt(typeNameIndex).isUtf8(),
                    "Invalid type name index at %d", typeNameIndex);
                throwValidExceptionAssert(
                    isValidCpIndex(constNameIndex) && cp->getTagAt(constNameIndex).isUtf8(),
                    "Invalid const name index at %d", constNameIndex);
                break;
            }
            case ElementValueTag::Class: {
                reader.ensure(2);
                auto classInfoIndex = reader.readU16Unchecked();
                throwValidExceptionAssert(
                    isValidCpIndex(classInfoIndex) && cp->getTagAt(classInfoIndex).isUtf8(),
                    "Invalid class info index at %d", classInfoIndex);
                break;
            }
            case ElementValueTag::AnnotationType: {
                /* auto annotation = */ parseAnnotation();

                break;
            }
            case ElementValueTag::ArrayType: {
                auto valueCount = reader.readU16();
                for (int i = 0; i < valueCount; i++) {
                    /*auto elementValue = */ parseElementValue();
                }
                break;
            }
            default:
                throwParseException("Invalid element tag value");
        }
    }

    const SymbolPtr &ClassFileParser::parseSignatureAttribute(uint32_t len) {
        throwValidExceptionAssert(len == 2, "Invalid signature attr length %d", len);
        auto sigIndex = reader.readU16Unchecked();
        throwValidExceptionAssert(
            isValidCpIndex(sigIndex)
            && cp->getTagAt(sigIndex).isUtf8(), "Invalid signature index %d", sigIndex);
        return cp->getSymbolAt(sigIndex);
    }

    void parseConstantValueAttr(uint32_t len) {

    }

    void ClassFileParser::parseInterfaces() noexcept(false) {
        CCW_ASSERT(interfaces.empty());

        auto interfacesCount = reader.readU16Unchecked();
        reader.ensure(2 * interfacesCount);
        for (int i = 0; i < interfacesCount; ++i) {
            auto index = reader.readU16Unchecked();
            throwValidExceptionAssert(isValidCpIndex(index) && cp->getTagAt(index).isClassOrUnresolvedClass(),
                                      "Invalid interface index at %d", index);
            interfaces.push_back(index);
        }
    }

    void ClassFileParser::parseConstantPool() noexcept(false) {
        CCW_ASSERT(cp == nullptr);

        reader.ensure(3);
        auto cpSize = reader.readU16Unchecked();
        cp = std::make_shared<ConstantPool>(cpSize);
        parseConstantPoolEntities();

        auto i = 0;
        while (++i < cpSize) {
            ConstantType tag = cp->getTagAt(i).type;
            switch (tag) {
                case ConstantType::Utf8:
                case ConstantType::Unicode:
                case ConstantType::Integer:
                case ConstantType::Float:
                    break;
                case ConstantType::Long:
                case ConstantType::Double:
                    i++;
                    break;
                case ConstantType::Class:
                    UNREACHABLE();
                case ConstantType::String:
                    UNREACHABLE();
                case ConstantType::Fieldref:
                case ConstantType::Methodref:
                case ConstantType::InterfaceMethodref: {
                    uint16_t classIndex = cp->getRefClassIndexAt(i);
                    throwValidExceptionAssert(
                        isValidCpIndex(classIndex) && cp->getTagAt(classIndex).isClassOrIndex(),
                        "Invalid class index at %d", classIndex);
                    uint16_t nameAndTypeIndex = cp->getRefNameAndTypeIndexAt(i);
                    throwValidExceptionAssert(
                        isValidCpIndex(nameAndTypeIndex) &&
                        cp->getTagAt(nameAndTypeIndex) == ConstantType::NameAndType,
                        "Invalid name and type index at %d", nameAndTypeIndex);
                    break;
                }
                case ConstantType::NameAndType: {
                    uint16_t nameIndex = cp->getNameAndTypeNameIndexAt(i);
                    throwValidExceptionAssert(
                        isValidCpIndex(nameIndex) && cp->getTagAt(nameIndex) == ConstantType::Utf8,
                        "Invalid name index at %d",
                        nameIndex);

                    uint16_t descriptorIndex = cp->getNameAndTypeDescriptorIndexAt(i);
                    throwValidExceptionAssert(
                        isValidCpIndex(descriptorIndex) && cp->getTagAt(descriptorIndex) == ConstantType::Utf8,
                        "Invalid descriptor index at %d",
                        descriptorIndex);
                }
                    break;
                case ConstantType::MethodHandle: {
                    auto kindValue = cp->getMethodHandleReferenceKindAt(i);
                    throwValidExceptionAssert(kindValue >= 1 && kindValue <= 9, "Invalid reference kind %d", kindValue);
                    auto referenceIndex = cp->getMethodHandleReferenceIndexAt(i);
                    throwValidExceptionAssert(isValidCpIndex(referenceIndex), "Invalid reference index %d",
                                              referenceIndex);
                    auto kind = static_cast<ReferenceKind >(kindValue);
                    switch (kind) {
                        case ReferenceKind::REF_getField:
                        case ReferenceKind::REF_getStatic:
                        case ReferenceKind::REF_putField:
                        case ReferenceKind::REF_putStatic:
                            throwValidExceptionAssert(cp->getTagAt(referenceIndex) == ConstantType::Fieldref,
                                                      "Invalid field reference index %d", kindValue);
                            break;
                        case ReferenceKind::REF_invokeVirtual:
                        case ReferenceKind::REF_newInvokeSpecial:
                            throwValidExceptionAssert(cp->getTagAt(referenceIndex) == ConstantType::Methodref,
                                                      "Invalid method reference index %d", kindValue);
                            break;
                        case ReferenceKind::REF_invokeStatic:
                        case ReferenceKind::REF_invokeSpecial: {
                            ConstantTag refTag = cp->getTagAt(referenceIndex);
                            throwValidExceptionAssert(
                                majorVersion < JAVA_8_VERSION && refTag == ConstantType::Methodref ||
                                majorVersion >= JAVA_8_VERSION &&
                                (refTag == ConstantType::Methodref || refTag == ConstantType::InterfaceMethodref),
                                "Invalid method reference index %d", kindValue);
                            break;
                        }
                        case ReferenceKind::REF_invokeInterface: {
                            throwValidExceptionAssert(cp->getTagAt(referenceIndex) == ConstantType::InterfaceMethodref,
                                                      "Invalid method reference index %d", kindValue);
                            break;
                        }
                        default:
                            throwParseException("Invalid reference kind value %d", kindValue);
                    }

                }
                    break;
                case ConstantType::MethodType: {
                    uint16_t descriptorIndex = cp->getMethodTypeDescriptorIndex(i);
                    throwValidExceptionAssert(
                        isValidCpIndex(descriptorIndex) && cp->getTagAt(descriptorIndex) == ConstantType::Utf8,
                        "Invalid descriptor index at %d",
                        descriptorIndex);
                    break;
                }
                case ConstantType::InvokeDynamic: {
                    uint16_t nameAndTypeIndex = cp->getInvokeDynamicNameAndTypeIndexAt(i);
                    throwValidExceptionAssert(
                        isValidCpIndex(nameAndTypeIndex) &&
                        cp->getTagAt(nameAndTypeIndex) == ConstantType::NameAndType,
                        "Invalid name and type index at %d", nameAndTypeIndex);
                    break;
                }
                case ConstantType::ClassIndex: {
                    uint16_t nameIndex = cp->getClassIndexAt(i);
                    throwValidExceptionAssert(
                        isValidCpIndex(nameIndex) && cp->getTagAt(nameIndex) == ConstantType::Utf8,
                        "Invalid class name index at %d", nameIndex);
                    const SymbolPtr &className = cp->getSymbolAt(nameIndex);
                    cp->putUnresolvedClassAt(i, className);
                    break;
                }
                case ConstantType::StringIndex: {
                    uint16_t stringIndex = cp->getStringIndexAt(i);
                    throwValidExceptionAssert(
                        isValidCpIndex(stringIndex) && cp->getTagAt(stringIndex) == ConstantType::Utf8,
                        "Invalid string index at %d", stringIndex);
                    const SymbolPtr &symbol = cp->getSymbolAt(stringIndex);
                    cp->putStringAt(i, symbol);
                    break;
                }
                case ConstantType::UnresolvedClass:
                    UNREACHABLE();
                    return;
                default:
                    UNREACHABLE();
                    return;
            }
        }

        // TODO validate cp
    }

    void ClassFileParser::parseConstantPoolEntities() noexcept(false) {
        uint16_t i = 0;
        auto cpSize = cp->getSize();
        while (++i < cpSize) {
            auto tagValue = reader.readU8Unchecked();
            switch (static_cast<ConstantType>(tagValue)) {
                case ConstantType::Class: {
                    auto nameIndex = reader.readU16();
                    cp->putClassIndexAt(i, nameIndex);
                    break;
                }
                case ConstantType::Fieldref: {
                    reader.ensure(5);
                    auto classIndex = reader.readU16Unchecked();
                    auto nameAndTypeIndex = reader.readU16Unchecked();
                    cp->putFieldRefAt(i, classIndex, nameAndTypeIndex);
                    break;
                }
                case ConstantType::Methodref: {
                    reader.ensure(5);
                    auto classIndex = reader.readU16Unchecked();
                    auto nameAndTypeIndex = reader.readU16Unchecked();
                    cp->putMethodRefAt(i, classIndex, nameAndTypeIndex);
                    break;
                }
                case ConstantType::InterfaceMethodref: {
                    reader.ensure(5);
                    auto classIndex = reader.readU16Unchecked();
                    auto nameAndTypeIndex = reader.readU16Unchecked();
                    cp->putInterfaceMethodRefAt(i, classIndex, nameAndTypeIndex);
                    break;
                }
                case ConstantType::String: {
                    reader.ensure(3);
                    auto index = reader.readU16Unchecked();
                    cp->putStringIndexAt(i, index);
                    break;
                }
                case ConstantType::Integer: {
                    reader.ensure(5);
                    auto intValue = reader.readU32Unchecked();
                    cp->putIntegerAt(i, intValue);
                    break;
                }
                case ConstantType::Float: {
                    reader.ensure(5);
                    auto floatValue = reader.readU32Unchecked();
                    cp->putFloatAt(i, floatValue);
                    break;
                }
                case ConstantType::Long: {
                    reader.ensure(9);
                    auto bytes = reader.readU64Unchecked();
                    cp->putLongAt(i, bytes);
                    i++;
                    break;
                }
                case ConstantType::Double: {
                    reader.ensure(9);
                    auto bytes = reader.readU64Unchecked();
                    cp->putDoubleAt(i, bytes);
                    i++;
                    break;
                }
                case ConstantType::NameAndType: {
                    reader.ensure(5);
                    auto nameIndex = reader.readU16Unchecked();
                    auto descriptor = reader.readU16Unchecked();
                    cp->putNameAndTypeAt(i, nameIndex, descriptor);
                    break;
                }
                case ConstantType::Utf8: {
                    auto len = reader.readU16();
                    reader.ensure(len + 1);
                    auto symbol = Symbol::create(reader.buffer(), len);
                    SymbolTable::putSymbol(symbol);
                    cp->putSymbolAt(i, symbol);
                    reader.skipUnchecked(len);
                    break;
                }
                case ConstantType::MethodHandle: {
                    reader.ensure(4);
                    auto referenceKind = reader.readU8Unchecked();
                    auto referenceIndex = reader.readU16Unchecked();
                    cp->putMethodHandleAt(i, referenceKind, referenceIndex);
                    break;
                }
                case ConstantType::MethodType: {
                    reader.ensure(3);
                    auto descriptorIndex = reader.readU16Unchecked();
                    cp->putMethodTypeAt(i, descriptorIndex);
                    break;
                }
                case ConstantType::InvokeDynamic: {
                    reader.ensure(5);
                    auto bootstrapMethodAttrIndex = reader.readU16Unchecked();
                    auto nameAndTypeIndex = reader.readU16Unchecked();
                    cp->putInvokeDynamicAt(i, bootstrapMethodAttrIndex, nameAndTypeIndex);
                    break;
                }
                default: {
                    throwParseException("Invalid constant type tag: %d at %d", tagValue, i);
                }
            }
        }
    }

    void ClassFileParser::parseTypeAnnotations() noexcept(false) {
        // TODO
    }
}