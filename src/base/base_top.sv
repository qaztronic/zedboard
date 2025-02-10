// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

module base_top
( inout [14:0] DDR_addr
, inout [ 2:0] DDR_ba
, inout        DDR_cas_n
, inout        DDR_ck_n
, inout        DDR_ck_p
, inout        DDR_cke
, inout        DDR_cs_n
, inout [ 3:0] DDR_dm
, inout [31:0] DDR_dq
, inout [ 3:0] DDR_dqs_n
, inout [ 3:0] DDR_dqs_p
, inout        DDR_odt
, inout        DDR_ras_n
, inout        DDR_reset_n
, inout        DDR_we_n
, inout        FIXED_IO_ddr_vrn
, inout        FIXED_IO_ddr_vrp
, inout [53:0] FIXED_IO_mio
, inout        FIXED_IO_ps_clk
, inout        FIXED_IO_ps_porb
, inout        FIXED_IO_ps_srstb
, output [7:0] fled
, inout  [10:1] JA
, inout  [10:1] JB
);
  // --------------------------------------------------------------------------------
  wire aclk   ;
  wire aresetn;

  base_cpu_w cpu_i
  ( .CAN_0_0_rx(JA[4])
  , .CAN_0_0_tx(JA[3])
  , .CAN_1_0_rx(JB[4])
  , .CAN_1_0_tx(JB[3])
  , .*
  );

  // --------------------------------------------------------------------------------
  reg [23:0] counter;
  assign fled = counter[23:16];

  always_ff @(posedge aclk)
    counter <= counter + 1;

// --------------------------------------------------------------------------------
endmodule
