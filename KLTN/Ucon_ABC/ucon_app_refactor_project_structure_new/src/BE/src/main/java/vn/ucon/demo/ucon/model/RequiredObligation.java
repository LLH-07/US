package vn.ucon.demo.ucon.model;

public record RequiredObligation(
        String obligationId,
        String type,
        String version,
        String policyRef,
        String title,
        String description
) {

    /*
     * Constructor này dùng cho AuthzForcePdpAdapter.
     * PDP chỉ trả thông tin policy-level, chưa có metadata hiển thị.
     */
    public RequiredObligation(
            String obligationId,
            String type,
            String version,
            String policyRef
    ) {
        this(
                obligationId,
                type,
                version,
                policyRef,
                null,
                null
        );
    }
}