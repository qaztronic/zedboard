// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

module base_cpu_w
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
, input        CAN_0_0_rx
, output       CAN_0_0_tx
, input        CAN_1_0_rx
, output       CAN_1_0_tx
, output       aclk
, output       aresetn
);
  // --------------------------------------------------------------------------------
  wire [31:0]M_AXI_GP0_0_araddr;
  wire [1:0]M_AXI_GP0_0_arburst;
  wire [3:0]M_AXI_GP0_0_arcache;
  wire [11:0]M_AXI_GP0_0_arid;
  wire [3:0]M_AXI_GP0_0_arlen;
  wire [1:0]M_AXI_GP0_0_arlock;
  wire [2:0]M_AXI_GP0_0_arprot;
  wire [3:0]M_AXI_GP0_0_arqos;
  wire M_AXI_GP0_0_arready = 1;
  wire [2:0]M_AXI_GP0_0_arsize;
  wire M_AXI_GP0_0_arvalid;
  wire [31:0]M_AXI_GP0_0_awaddr;
  wire [1:0]M_AXI_GP0_0_awburst;
  wire [3:0]M_AXI_GP0_0_awcache;
  wire [11:0]M_AXI_GP0_0_awid;
  wire [3:0]M_AXI_GP0_0_awlen;
  wire [1:0]M_AXI_GP0_0_awlock;
  wire [2:0]M_AXI_GP0_0_awprot;
  wire [3:0]M_AXI_GP0_0_awqos;
  wire M_AXI_GP0_0_awready = 1;
  wire [2:0]M_AXI_GP0_0_awsize;
  wire M_AXI_GP0_0_awvalid;
  wire [11:0]M_AXI_GP0_0_bid = 0;
  wire M_AXI_GP0_0_bready;
  wire [1:0]M_AXI_GP0_0_bresp = 0;
  wire M_AXI_GP0_0_bvalid = 1;
  wire [31:0]M_AXI_GP0_0_rdata = 0;
  wire [11:0]M_AXI_GP0_0_rid = 0;
  wire M_AXI_GP0_0_rlast = 0;
  wire M_AXI_GP0_0_rready;
  wire [1:0]M_AXI_GP0_0_rresp = 0;
  wire M_AXI_GP0_0_rvalid = 1;
  wire [31:0]M_AXI_GP0_0_wdata;
  wire [11:0]M_AXI_GP0_0_wid;
  wire M_AXI_GP0_0_wlast;
  wire M_AXI_GP0_0_wready = 1;
  wire [3:0]M_AXI_GP0_0_wstrb;
  wire M_AXI_GP0_0_wvalid;
  
  base_cpu base_cpu_i(.*);

// --------------------------------------------------------------------------------
endmodule
