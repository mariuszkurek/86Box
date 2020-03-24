/*
 * 86Box	A hypervisor and IBM PC system emulator that specializes in
 *		running old operating systems and software designed for IBM
 *		PC systems and compatibles from 1981 through fairly recent
 *		system designs based on the PCI bus.
 *
 *		This file is part of the 86Box distribution.
 *
 *		Common 386 CPU code.
 *
 * Version:	@(#)386_common.h	1.0.1	2019/02/19
 *
 * Author:	Sarah Walker, <http://pcem-emulator.co.uk/>
 *		Miran Grca, <mgrca8@gmail.com>
 *		Copyright 2008-2019 Sarah Walker.
 *		Copyright 2016-2019 Miran Grca.
 */

#ifndef _386_COMMON_H_
#define _386_COMMON_H_

#ifdef USE_NEW_DYNAREC
#define readmemb(s,a) ((readlookup2[(uint32_t)((s)+(a))>>12]==-1)?readmembl((s)+(a)): *(uint8_t *)(readlookup2[(uint32_t)((s)+(a))>>12] + (uint32_t)((s) + (a))) )
#define readmemw(s,a) ((readlookup2[(uint32_t)((s)+(a))>>12]==-1 || (((s)+(a)) & 1))?readmemwl((s)+(a)):*(uint16_t *)(readlookup2[(uint32_t)((s)+(a))>>12]+(uint32_t)((s)+(a))))
#define readmeml(s,a) ((readlookup2[(uint32_t)((s)+(a))>>12]==-1 || (((s)+(a)) & 3))?readmemll((s)+(a)):*(uint32_t *)(readlookup2[(uint32_t)((s)+(a))>>12]+(uint32_t)((s)+(a))))
#define readmemq(s,a) ((readlookup2[(uint32_t)((s)+(a))>>12]==-1 || (((s)+(a)) & 7))?readmemql((s)+(a)):*(uint64_t *)(readlookup2[(uint32_t)((s)+(a))>>12]+(uint32_t)((s)+(a))))

#define writememb(s,a,v) if (writelookup2[(uint32_t)((s)+(a))>>12]==-1) writemembl((s)+(a),v); else *(uint8_t *)(writelookup2[(uint32_t)((s) + (a)) >> 12] + (uint32_t)((s) + (a))) = v
#define writememw(s,a,v) if (writelookup2[(uint32_t)((s)+(a))>>12]==-1 || (((s)+(a)) & 1)) writememwl((s)+(a),v); else *(uint16_t *)(writelookup2[(uint32_t)((s) + (a)) >> 12] + (uint32_t)((s) + (a))) = v
#define writememl(s,a,v) if (writelookup2[(uint32_t)((s)+(a))>>12]==-1 || (((s)+(a)) & 3)) writememll((s)+(a),v); else *(uint32_t *)(writelookup2[(uint32_t)((s) + (a)) >> 12] + (uint32_t)((s) + (a))) = v
#define writememq(s,a,v) if (writelookup2[(uint32_t)((s)+(a))>>12]==-1 || (((s)+(a)) & 7)) writememql((s)+(a),v); else *(uint64_t *)(writelookup2[(uint32_t)((s) + (a)) >> 12] + (uint32_t)((s) + (a))) = v
#else
#undef readmemb
#undef writememb


#define readmemb(s,a) ((readlookup2[(uint32_t)((s)+(a))>>12]==-1 || (s)==0xFFFFFFFF)?readmemb386l(s,a): *(uint8_t *)(readlookup2[(uint32_t)((s)+(a))>>12] + (uint32_t)((s) + (a))) )
#define readmemq(s,a) ((readlookup2[(uint32_t)((s)+(a))>>12]==-1 || (s)==0xFFFFFFFF || (((s)+(a)) & 7))?readmemql(s,a):*(uint64_t *)(readlookup2[(uint32_t)((s)+(a))>>12]+(uint32_t)((s)+(a))))

#define writememb(s,a,v) if (writelookup2[(uint32_t)((s)+(a))>>12]==-1 || (s)==0xFFFFFFFF) writememb386l(s,a,v); else *(uint8_t *)(writelookup2[(uint32_t)((s) + (a)) >> 12] + (uint32_t)((s) + (a))) = v

#define writememw(s,a,v) if (writelookup2[(uint32_t)((s)+(a))>>12]==-1 || (s)==0xFFFFFFFF || (((s)+(a)) & 1)) writememwl(s,a,v); else *(uint16_t *)(writelookup2[(uint32_t)((s) + (a)) >> 12] + (uint32_t)((s) + (a))) = v
#define writememl(s,a,v) if (writelookup2[(uint32_t)((s)+(a))>>12]==-1 || (s)==0xFFFFFFFF || (((s)+(a)) & 3)) writememll(s,a,v); else *(uint32_t *)(writelookup2[(uint32_t)((s) + (a)) >> 12] + (uint32_t)((s) + (a))) = v
#define writememq(s,a,v) if (writelookup2[(uint32_t)((s)+(a))>>12]==-1 || (s)==0xFFFFFFFF || (((s)+(a)) & 7)) writememql(s,a,v); else *(uint64_t *)(writelookup2[(uint32_t)((s) + (a)) >> 12] + (uint32_t)((s) + (a))) = v
#endif


int checkio(int port);


#ifdef USE_NEW_DYNAREC
#define check_io_perm(port) if (!IOPLp || (cpu_state.eflags&VM_FLAG)) \
                        { \
                                int tempi = checkio(port); \
                                if (cpu_state.abrt) return 1; \
                                if (tempi) \
                                { \
                                        x86gpf("check_io_perm(): no permission",0); \
                                        return 1; \
                                } \
                        }
#else
#define check_io_perm(port) if (msw&1 && ((CPL > IOPL) || (cpu_state.eflags&VM_FLAG))) \
                        { \
                                int tempi = checkio(port); \
                                if (cpu_state.abrt) return 1; \
                                if (tempi) \
                                { \
                                        x86gpf("check_io_perm(): no permission",0); \
                                        return 1; \
                                } \
                        }
#endif

#define SEG_CHECK_READ(seg)                             \
        do                                              \
        {                                               \
                if ((seg)->base == 0xffffffff)          \
                {                                       \
                        x86gpf("Segment can't read", 0);\
                        return 1;                       \
                }                                       \
        } while (0)

#define SEG_CHECK_WRITE(seg)                    \
        do                                              \
        {                                               \
                if ((seg)->base == 0xffffffff)          \
                {                                       \
                        x86gpf("Segment can't write", 0);\
                        return 1;                       \
                }                                       \
        } while (0)

#define CHECK_READ(chseg, low, high)  \
        if ((low < (chseg)->limit_low) || (high > (chseg)->limit_high) || ((msw & 1) && !(cpu_state.eflags & VM_FLAG) && (((chseg)->access & 10) == 8)))       \
        {                                       \
                x86gpf("Limit check (READ)", 0);       \
                return 1;                       \
	}					\
	if (msw&1 && !(cpu_state.eflags&VM_FLAG) && !((chseg)->access & 0x80))	\
	{					\
		if ((chseg) == &cpu_state.seg_ss)		\
			x86ss(NULL,(chseg)->seg & 0xfffc);	\
		else				\
			x86np("Read from seg not present", (chseg)->seg & 0xfffc);	\
		return 1;			\
        }					\
	if (cr0 >> 31) {			\
		(void) mmutranslatereal((chseg)->base + low, 0);	\
		(void) mmutranslatereal((chseg)->base + high, 0);	\
		if (cpu_state.abrt)		\
			return 1;		\
	}

#define CHECK_READ_REP(chseg, low, high)  \
        if ((low < (chseg)->limit_low) || (high > (chseg)->limit_high))       \
        {                                       \
                x86gpf("Limit check (READ)", 0);       \
                break;                       \
	}					\
	if (msw&1 && !(cpu_state.eflags&VM_FLAG) && !((chseg)->access & 0x80))	\
	{					\
		if ((chseg) == &cpu_state.seg_ss)		\
			x86ss(NULL,(chseg)->seg & 0xfffc);	\
		else				\
			x86np("Read from seg not present", (chseg)->seg & 0xfffc);	\
		break;			\
        }				\
	if (cr0 >> 31) {		\
		(void) mmutranslatereal((chseg)->base + low, 0);	\
		(void) mmutranslatereal((chseg)->base + high, 0);	\
		if (cpu_state.abrt)	\
			break;		\
	}

#define CHECK_WRITE_COMMON(chseg, low, high)  \
        if ((low < (chseg)->limit_low) || (high > (chseg)->limit_high) || !((chseg)->access & 2) || ((msw & 1) && !(cpu_state.eflags & VM_FLAG) && ((chseg)->access & 8)))       \
        {                                       \
                x86gpf("Limit check (WRITE)", 0);       \
                return 1;                       \
	}					\
	if (msw&1 && !(cpu_state.eflags&VM_FLAG) && !((chseg)->access & 0x80))	\
	{					\
		if ((chseg) == &cpu_state.seg_ss)		\
			x86ss(NULL,(chseg)->seg & 0xfffc);	\
		else				\
			x86np("Write to seg not present", (chseg)->seg & 0xfffc);	\
		return 1;			\
	}

#define CHECK_WRITE(chseg, low, high)  \
	CHECK_WRITE_COMMON(chseg, low, high)	\
	if (cr0 >> 31) {			\
		(void) mmutranslatereal((chseg)->base + low, 1);	\
		(void) mmutranslatereal((chseg)->base + high, 1);	\
		if (cpu_state.abrt)		\
			return 1;		\
	}

#define CHECK_WRITE_REP(chseg, low, high)  \
        if ((low < (chseg)->limit_low) || (high > (chseg)->limit_high))       \
        {                                       \
                x86gpf("Limit check (WRITE REP)", 0);       \
                break;                       \
	}					\
	if (msw&1 && !(cpu_state.eflags&VM_FLAG) && !((chseg)->access & 0x80))	\
	{					\
		if ((chseg) == &cpu_state.seg_ss)		\
			x86ss(NULL,(chseg)->seg & 0xfffc);	\
		else				\
			x86np("Write (REP) to seg not present", (chseg)->seg & 0xfffc);	\
		break;                       \
        }					\
	if (cr0 >> 31) {			\
		(void) mmutranslatereal((chseg)->base + low, 1);	\
		(void) mmutranslatereal((chseg)->base + high, 1);	\
		if (cpu_state.abrt)		\
			break;			\
	}


#define NOTRM   if (!(msw & 1) || (cpu_state.eflags & VM_FLAG))\
                { \
                        x86_int(6); \
                        return 1; \
                }




static __inline uint8_t fastreadb(uint32_t a)
{
        uint8_t *t;
        
        if ((a >> 12) == pccache) 
                return *((uint8_t *)&pccache2[a]);
        t = getpccache(a);
        if (cpu_state.abrt)
                return 0;
        pccache = a >> 12;
        pccache2 = t;
        return *((uint8_t *)&pccache2[a]);
}

static __inline uint16_t fastreadw(uint32_t a)
{
        uint8_t *t;
        uint16_t val;
        if ((a&0xFFF)>0xFFE)
        {
                val = fastreadb(a);
                val |= (fastreadb(a + 1) << 8);
                return val;
        }
        if ((a>>12)==pccache) return *((uint16_t *)&pccache2[a]);
        t = getpccache(a);
        if (cpu_state.abrt)
                return 0;

        pccache = a >> 12;
        pccache2 = t;
        return *((uint16_t *)&pccache2[a]);
}

static __inline uint32_t fastreadl(uint32_t a)
{
        uint8_t *t;
        uint32_t val;
        if ((a&0xFFF)<0xFFD)
        {
                if ((a>>12)!=pccache)
                {
                        t = getpccache(a);
                        if (cpu_state.abrt)
                                return 0;
                        pccache2 = t;
                        pccache=a>>12;
                }
                return *((uint32_t *)&pccache2[a]);
        }
        val  = fastreadw(a);
        val |= (fastreadw(a + 2) << 16);
        return val;
}

static __inline void *get_ram_ptr(uint32_t a)
{
        if ((a >> 12) == pccache)
                return &pccache2[a];
        else
        {
                uint8_t *t = getpccache(a);
                return &t[a];
        }
}

static __inline uint8_t getbyte()
{
        cpu_state.pc++;
        return fastreadb(cs + (cpu_state.pc - 1));
}

static __inline uint16_t getword()
{
        cpu_state.pc+=2;
        return fastreadw(cs+(cpu_state.pc-2));
}

static __inline uint32_t getlong()
{
        cpu_state.pc+=4;
        return fastreadl(cs+(cpu_state.pc-4));
}

static __inline uint64_t getquad()
{
        cpu_state.pc+=8;
        return fastreadl(cs+(cpu_state.pc-8)) | ((uint64_t)fastreadl(cs+(cpu_state.pc-4)) << 32);
}



static __inline uint8_t geteab()
{
        if (cpu_mod == 3)
                return (cpu_rm & 4) ? cpu_state.regs[cpu_rm & 3].b.h : cpu_state.regs[cpu_rm&3].b.l;
        if (eal_r)
                return *(uint8_t *)eal_r;
        return readmemb(easeg, cpu_state.eaaddr);
}

static __inline uint16_t geteaw()
{
        if (cpu_mod == 3)
                return cpu_state.regs[cpu_rm].w;
        if (eal_r)
                return *(uint16_t *)eal_r;
        return readmemw(easeg, cpu_state.eaaddr);
}

static __inline uint32_t geteal()
{
        if (cpu_mod == 3)
                return cpu_state.regs[cpu_rm].l;
        if (eal_r)
                return *eal_r;
        return readmeml(easeg, cpu_state.eaaddr);
}

static __inline uint64_t geteaq()
{
        return readmemq(easeg, cpu_state.eaaddr);
}

static __inline uint8_t geteab_mem()
{
        if (eal_r) return *(uint8_t *)eal_r;
        return readmemb(easeg,cpu_state.eaaddr);
}
static __inline uint16_t geteaw_mem()
{
        if (eal_r) return *(uint16_t *)eal_r;
        return readmemw(easeg,cpu_state.eaaddr);
}
static __inline uint32_t geteal_mem()
{
        if (eal_r) return *eal_r;
        return readmeml(easeg,cpu_state.eaaddr);
}

static __inline int seteaq_cwc(void)
{
	CHECK_WRITE_COMMON(cpu_state.ea_seg, cpu_state.eaaddr, cpu_state.eaaddr);
	return 0;
}

static __inline void seteaq(uint64_t v)
{
	if (seteaq_cwc())
		return;
#ifdef USE_NEW_DYNAREC
        writememql(easeg + cpu_state.eaaddr, v);
#else
        writememql(easeg, cpu_state.eaaddr, v);
#endif
}

#ifdef USE_NEW_DYNAREC
#define seteab(v) if (cpu_mod!=3) { CHECK_WRITE_COMMON(cpu_state.ea_seg, cpu_state.eaaddr, cpu_state.eaaddr); if (eal_w) *(uint8_t *)eal_w=v;  else writemembl(easeg+cpu_state.eaaddr,v); } else if (cpu_rm&4) cpu_state.regs[cpu_rm&3].b.h=v; else cpu_state.regs[cpu_rm].b.l=v
#define seteaw(v) if (cpu_mod!=3) { CHECK_WRITE_COMMON(cpu_state.ea_seg, cpu_state.eaaddr, cpu_state.eaaddr + 1); if (eal_w) *(uint16_t *)eal_w=v; else writememwl(easeg+cpu_state.eaaddr,v);    } else cpu_state.regs[cpu_rm].w=v
#define seteal(v) if (cpu_mod!=3) { CHECK_WRITE_COMMON(cpu_state.ea_seg, cpu_state.eaaddr, cpu_state.eaaddr + 3); if (eal_w) *eal_w=v;             else writememll(easeg+cpu_state.eaaddr,v);    } else cpu_state.regs[cpu_rm].l=v
#else
#define seteab(v) if (cpu_mod!=3) { CHECK_WRITE_COMMON(cpu_state.ea_seg, cpu_state.eaaddr, cpu_state.eaaddr); if (eal_w) *(uint8_t *)eal_w=v;  else { writememb386l(easeg,cpu_state.eaaddr,v); } } else if (cpu_rm&4) cpu_state.regs[cpu_rm&3].b.h=v; else cpu_state.regs[cpu_rm].b.l=v
#define seteaw(v) if (cpu_mod!=3) { CHECK_WRITE_COMMON(cpu_state.ea_seg, cpu_state.eaaddr, cpu_state.eaaddr + 1); if (eal_w) *(uint16_t *)eal_w=v; else { writememwl(easeg,cpu_state.eaaddr,v); } } else cpu_state.regs[cpu_rm].w=v
#define seteal(v) if (cpu_mod!=3) { CHECK_WRITE_COMMON(cpu_state.ea_seg, cpu_state.eaaddr, cpu_state.eaaddr + 3); if (eal_w) *eal_w=v;             else { writememll(easeg,cpu_state.eaaddr,v); } } else cpu_state.regs[cpu_rm].l=v
#endif

#ifdef USE_NEW_DYNAREC
#define seteab_mem(v) if (eal_w) *(uint8_t *)eal_w=v;  else writemembl(easeg+cpu_state.eaaddr,v);
#define seteaw_mem(v) if (eal_w) *(uint16_t *)eal_w=v; else writememwl(easeg+cpu_state.eaaddr,v);
#define seteal_mem(v) if (eal_w) *eal_w=v;             else writememll(easeg+cpu_state.eaaddr,v);
#else
#define seteab_mem(v) if (eal_w) *(uint8_t *)eal_w=v;  else writememb386l(easeg,cpu_state.eaaddr,v);
#define seteaw_mem(v) if (eal_w) *(uint16_t *)eal_w=v; else writememwl(easeg,cpu_state.eaaddr,v);
#define seteal_mem(v) if (eal_w) *eal_w=v;             else writememll(easeg,cpu_state.eaaddr,v);
#endif

#define getbytef() ((uint8_t)(fetchdat)); cpu_state.pc++
#define getwordf() ((uint16_t)(fetchdat)); cpu_state.pc+=2
#define getbyte2f() ((uint8_t)(fetchdat>>8)); cpu_state.pc++
#define getword2f() ((uint16_t)(fetchdat>>8)); cpu_state.pc+=2

#endif