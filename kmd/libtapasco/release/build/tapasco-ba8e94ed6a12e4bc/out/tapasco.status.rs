#[derive(Clone, PartialEq, ::prost::Message)]
pub struct Interrupt {
    #[prost(uint64, tag="1")]
    pub mapping: u64,
    #[prost(string, tag="2")]
    pub name: std::string::String,
}
#[derive(Clone, PartialEq, ::prost::Message)]
pub struct Pe {
    #[prost(string, tag="1")]
    pub name: std::string::String,
    #[prost(uint32, tag="2")]
    pub id: u32,
    #[prost(uint64, tag="3")]
    pub offset: u64,
    #[prost(uint64, tag="4")]
    pub size: u64,
    #[prost(message, optional, tag="5")]
    pub local_memory: ::std::option::Option<MemoryArea>,
    #[prost(message, optional, tag="6")]
    pub debug: ::std::option::Option<Platform>,
    #[prost(message, repeated, tag="7")]
    pub interrupts: ::std::vec::Vec<Interrupt>,
}
#[derive(Clone, PartialEq, ::prost::Message)]
pub struct Platform {
    #[prost(string, tag="1")]
    pub name: std::string::String,
    #[prost(uint64, tag="2")]
    pub offset: u64,
    #[prost(uint64, tag="3")]
    pub size: u64,
    #[prost(message, repeated, tag="4")]
    pub interrupts: ::std::vec::Vec<Interrupt>,
}
#[derive(Clone, PartialEq, ::prost::Message)]
pub struct Clock {
    #[prost(string, tag="1")]
    pub name: std::string::String,
    #[prost(uint32, tag="2")]
    pub frequency_mhz: u32,
}
#[derive(Clone, PartialEq, ::prost::Message)]
pub struct Version {
    #[prost(string, tag="1")]
    pub software: std::string::String,
    #[prost(uint32, tag="2")]
    pub year: u32,
    #[prost(uint32, tag="3")]
    pub release: u32,
    #[prost(string, tag="4")]
    pub extra_version: std::string::String,
}
#[derive(Clone, PartialEq, ::prost::Message)]
pub struct MemoryArea {
    #[prost(uint64, tag="1")]
    pub base: u64,
    #[prost(uint64, tag="2")]
    pub size: u64,
}
#[derive(Clone, PartialEq, ::prost::Message)]
pub struct Status {
    #[prost(uint64, tag="1")]
    pub timestamp: u64,
    #[prost(message, optional, tag="2")]
    pub arch_base: ::std::option::Option<MemoryArea>,
    #[prost(message, optional, tag="3")]
    pub platform_base: ::std::option::Option<MemoryArea>,
    #[prost(message, repeated, tag="4")]
    pub pe: ::std::vec::Vec<Pe>,
    #[prost(message, repeated, tag="5")]
    pub platform: ::std::vec::Vec<Platform>,
    #[prost(message, repeated, tag="6")]
    pub clocks: ::std::vec::Vec<Clock>,
    #[prost(message, repeated, tag="7")]
    pub versions: ::std::vec::Vec<Version>,
}
