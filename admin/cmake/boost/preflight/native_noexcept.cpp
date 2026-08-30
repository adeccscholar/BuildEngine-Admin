struct base_ty {
   virtual ~base_ty() = default;
   virtual int Value() const noexcept = 0;
};
struct derived_ty final : base_ty {
   int Value() const noexcept override { return 23; }
};
static_assert(noexcept(derived_ty {}.Value()));
int ProbeNativeNoexcept() { return derived_ty {}.Value(); }
