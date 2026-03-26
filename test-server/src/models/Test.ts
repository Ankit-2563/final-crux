import mongoose, { Schema, Document } from "mongoose";

export interface ITest extends Document {
  value: number;
  deviceId: string;
  timestamp: Date;
}

const testSchema = new Schema<ITest>(
  {
    value: {
      type: Number,
      required: true,
      min: 1,
      max: 10,
    },
    deviceId: {
      type: String,
      required: true,
    },
    timestamp: {
      type: Date,
      default: Date.now,
    },
  },
  {
    collection: "test", // Explicit collection name
  }
);

const Test = mongoose.model<ITest>("Test", testSchema);

export default Test;
