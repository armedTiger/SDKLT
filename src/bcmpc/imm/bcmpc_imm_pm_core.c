/*! \file bcmpc_imm_pm_core.c
 *
 * PC_PM_COREt IMM handler.
 */
/*
 * Copyright: (c) 2018 Broadcom. All Rights Reserved. "Broadcom" refers to 
 * Broadcom Limited and/or its subsidiaries.
 * 
 * Broadcom Switch Software License
 * 
 * This license governs the use of the accompanying Broadcom software. Your 
 * use of the software indicates your acceptance of the terms and conditions 
 * of this license. If you do not agree to the terms and conditions of this 
 * license, do not use the software.
 * 1. Definitions
 *    "Licensor" means any person or entity that distributes its Work.
 *    "Software" means the original work of authorship made available under 
 *    this license.
 *    "Work" means the Software and any additions to or derivative works of 
 *    the Software that are made available under this license.
 *    The terms "reproduce," "reproduction," "derivative works," and 
 *    "distribution" have the meaning as provided under U.S. copyright law.
 *    Works, including the Software, are "made available" under this license 
 *    by including in or with the Work either (a) a copyright notice 
 *    referencing the applicability of this license to the Work, or (b) a copy 
 *    of this license.
 * 2. Grant of Copyright License
 *    Subject to the terms and conditions of this license, each Licensor 
 *    grants to you a perpetual, worldwide, non-exclusive, and royalty-free 
 *    copyright license to reproduce, prepare derivative works of, publicly 
 *    display, publicly perform, sublicense and distribute its Work and any 
 *    resulting derivative works in any form.
 * 3. Grant of Patent License
 *    Subject to the terms and conditions of this license, each Licensor 
 *    grants to you a perpetual, worldwide, non-exclusive, and royalty-free 
 *    patent license to make, have made, use, offer to sell, sell, import, and 
 *    otherwise transfer its Work, in whole or in part. This patent license 
 *    applies only to the patent claims licensable by Licensor that would be 
 *    infringed by Licensor's Work (or portion thereof) individually and 
 *    excluding any combinations with any other materials or technology.
 *    If you institute patent litigation against any Licensor (including a 
 *    cross-claim or counterclaim in a lawsuit) to enforce any patents that 
 *    you allege are infringed by any Work, then your patent license from such 
 *    Licensor to the Work shall terminate as of the date such litigation is 
 *    filed.
 * 4. Redistribution
 *    You may reproduce or distribute the Work only if (a) you do so under 
 *    this License, (b) you include a complete copy of this License with your 
 *    distribution, and (c) you retain without modification any copyright, 
 *    patent, trademark, or attribution notices that are present in the Work.
 * 5. Derivative Works
 *    You may specify that additional or different terms apply to the use, 
 *    reproduction, and distribution of your derivative works of the Work 
 *    ("Your Terms") only if (a) Your Terms provide that the limitations of 
 *    Section 7 apply to your derivative works, and (b) you identify the 
 *    specific derivative works that are subject to Your Terms. 
 *    Notwithstanding Your Terms, this license (including the redistribution 
 *    requirements in Section 4) will continue to apply to the Work itself.
 * 6. Trademarks
 *    This license does not grant any rights to use any Licensor's or its 
 *    affiliates' names, logos, or trademarks, except as necessary to 
 *    reproduce the notices described in this license.
 * 7. Limitations
 *    Platform. The Work and any derivative works thereof may only be used, or 
 *    intended for use, with a Broadcom switch integrated circuit.
 *    No Reverse Engineering. You will not use the Work to disassemble, 
 *    reverse engineer, decompile, or attempt to ascertain the underlying 
 *    technology of a Broadcom switch integrated circuit.
 * 8. Termination
 *    If you violate any term of this license, then your rights under this 
 *    license (including the license grants of Sections 2 and 3) will 
 *    terminate immediately.
 * 9. Disclaimer of Warranty
 *    THE WORK IS PROVIDED "AS IS" WITHOUT WARRANTIES OR CONDITIONS OF ANY 
 *    KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WARRANTIES OR CONDITIONS OF 
 *    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE OR 
 *    NON-INFRINGEMENT. YOU BEAR THE RISK OF UNDERTAKING ANY ACTIVITIES UNDER 
 *    THIS LICENSE. SOME STATES' CONSUMER LAWS DO NOT ALLOW EXCLUSION OF AN 
 *    IMPLIED WARRANTY, SO THIS DISCLAIMER MAY NOT APPLY TO YOU.
 * 10. Limitation of Liability
 *    EXCEPT AS PROHIBITED BY APPLICABLE LAW, IN NO EVENT AND UNDER NO LEGAL 
 *    THEORY, WHETHER IN TORT (INCLUDING NEGLIGENCE), CONTRACT, OR OTHERWISE 
 *    SHALL ANY LICENSOR BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY DIRECT, 
 *    INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF 
 *    OR RELATED TO THIS LICENSE, THE USE OR INABILITY TO USE THE WORK 
 *    (INCLUDING BUT NOT LIMITED TO LOSS OF GOODWILL, BUSINESS INTERRUPTION, 
 *    LOST PROFITS OR DATA, COMPUTER FAILURE OR MALFUNCTION, OR ANY OTHER 
 *    COMMERCIAL DAMAGES OR LOSSES), EVEN IF THE LICENSOR HAS BEEN ADVISED OF 
 *    THE POSSIBILITY OF SUCH DAMAGES.
 */


#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmimm/bcmimm_int_comp.h>

#include <bcmpc/bcmpc_db_internal.h>
#include <bcmpc/bcmpc_imm_internal.h>
#include <bcmpc/bcmpc_pm.h>

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_IMM

/*! Logical table ID for this handler. */
#define IMM_SID PC_PM_COREt


/*******************************************************************************
 * DB schema handler
 */

/*!
 * \brief Convert the LT fields to the PC data structure.
 *
 * This is the parse callback function of IMM DB schema handler, \ref
 * bcmpc_db_imm_schema_handler_t.
 *
 * Convert bcmltd_fields_t array \c flds to a bcmpc_pm_core_t \c key and a
 * bcmpc_pm_core_cfg_t \c data.
 *
 * \param [in] unit Unit number.
 * \param [in] flds LT field array.
 * \param [in] key_size Key size.
 * \param [out] key Key value from \c flds.
 * \param [in] data_size Data size.
 * \param [out] data Data value from \c flds.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
pc_pm_core_entry_parse(int unit, const bcmltd_fields_t *flds,
                       size_t key_size, void *key,
                       size_t data_size, void *data)
{
    size_t i;
    uint32_t fid;
    uint64_t fval;
    bcmpc_pm_core_t *pm_core = key;
    bcmpc_pm_core_cfg_t *ccfg = data;

    SHR_FUNC_ENTER(unit);

    if ((key && key_size != sizeof(*pm_core)) ||
        (data && data_size != sizeof(*ccfg))) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    bcmpc_pm_core_cfg_t_init(ccfg);

    for (i = 0; i < flds->count; i++) {
        fid = flds->field[i]->id;
        fval = flds->field[i]->data;

        switch (fid) {
            case PC_PM_COREt_PC_PM_IDf:
                pm_core->pm_id = fval;
                break;
            case PC_PM_COREt_CORE_INDEXf:
                pm_core->core_idx = fval;
                break;
            case PC_PM_COREt_CORE_CLKf:
                ccfg->core_clk = fval;
                break;
            case PC_PM_COREt_TX_LANE_MAPf:
                ccfg->tx_lane_map = fval;
                break;
            case PC_PM_COREt_RX_LANE_MAPf:
                ccfg->rx_lane_map = fval;
                break;
            case PC_PM_COREt_TX_POLARITY_FLIPf:
                ccfg->tx_polarity_flip = fval;
                break;
            case PC_PM_COREt_RX_POLARITY_FLIPf:
                ccfg->rx_polarity_flip = fval;
                break;
            default:
                SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Convert the PC data structure to the LT fields.
 *
 * This is the fill callback function of IMM DB schema handler, \ref
 * bcmpc_db_imm_schema_handler_t.
 *
 * Convert a bcmpc_pm_core_t \c key and a bcmpc_pm_core_cfg_t \c data to
 * bcmltd_fields_t array \c flds.
 *
 * \param [in] unit Unit number.
 * \param [in] key_size Key size.
 * \param [in] key Key value.
 * \param [in] data_size Data size.
 * \param [in] data Data value.
 * \param [out] flds LT field array.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
pc_pm_core_entry_fill(int unit,
                      size_t key_size, void *key,
                      size_t data_size, void *data,
                      bcmltd_fields_t *flds)
{
    size_t fcnt = 0;
    bcmpc_pm_core_t *pm_core = key;
    bcmpc_pm_core_cfg_t *ccfg = data;

    SHR_FUNC_ENTER(unit);

    if ((key && key_size != sizeof(*pm_core)) ||
        (data && data_size != sizeof(*ccfg))) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    if (key) {
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PM_COREt_PC_PM_IDf, 0, pm_core->pm_id, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PM_COREt_CORE_INDEXf, 0, pm_core->core_idx, fcnt);
    }

    if (data) {
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PM_COREt_CORE_CLKf, 0, ccfg->core_clk, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PM_COREt_TX_LANE_MAPf, 0, ccfg->tx_lane_map, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PM_COREt_RX_LANE_MAPf, 0, ccfg->rx_lane_map, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PM_COREt_TX_POLARITY_FLIPf, 0, ccfg->tx_polarity_flip,
             fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PM_COREt_RX_POLARITY_FLIPf, 0, ccfg->rx_polarity_flip,
             fcnt);
    }

    flds->count = fcnt;

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public Functions
 */

int
bcmpc_imm_pm_core_register(int unit)
{
    bcmpc_db_imm_schema_handler_t db_hdl;

    SHR_FUNC_ENTER(unit);

    db_hdl.entry_parse = pc_pm_core_entry_parse;
    db_hdl.entry_fill = pc_pm_core_entry_fill;

    /* Register the handlers to the table. */
    SHR_IF_ERR_EXIT
        (bcmpc_imm_tbl_register(unit, IMM_SID, &db_hdl, NULL));

exit:
    SHR_FUNC_EXIT();
}
